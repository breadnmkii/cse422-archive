#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/memory.h>
#include <linux/mm.h>

#include <paging.h>

// demand paging module parameter
static unsigned int demand_paging = 1;
module_param(demand_paging, uint, 0644);

// struct for tracking pa allocs for procs (in vm_private_data)
struct pa_state_struct {
    atomic_t ref_count;
    struct ref_page_node * ref_page;
};

// Head node of llist of page references
LIST_HEAD(ref_page_list);

// struct for linked list of page references
struct ref_page_node {
  struct page * page;
  struct list_head list;
};

// verification counters
static atomic_t alloc_count;
static atomic_t free_count;

static int
do_fault(struct vm_area_struct * vma,
         unsigned long           fault_address)
{
    struct ref_page_node * page_node;
    struct page * pg;

    if (demand_paging) {
        printk(KERN_INFO "paging_vma_fault() invoked: took a page fault at VA 0x%lx\n", fault_address);
    } else {
        printk(KERN_INFO "prepaging invoked: allocating single page at VA 0x%lx\n", fault_address);
    }

    // aloc single page
    if (!(pg = alloc_page(GFP_KERNEL))) {
        return VM_FAULT_OOM;
    }
    // update proc page table
    if (0 != remap_pfn_range(vma, PAGE_ALIGN(fault_address), page_to_pfn(pg), PAGE_SIZE, vma->vm_page_prot)) {
        __free_page(pg);
        return VM_FAULT_SIGBUS;
    }

    // init page list node
    page_node = kmalloc(sizeof(*page_node), GFP_KERNEL);
    page_node->page = pg;
    INIT_LIST_HEAD(&(page_node->list));

    // store page node into pa_state_struct's ref_page_node llist
    list_add(&(page_node->list), &ref_page_list);

    atomic_inc(&alloc_count);

    return VM_FAULT_NOPAGE;
}

static vm_fault_t
paging_vma_fault(struct vm_fault * vmf)
{
    struct vm_area_struct * vma = vmf->vma;
    unsigned long fault_address = (unsigned long)vmf->address;

    return do_fault(vma, fault_address);
}

static void
paging_vma_open(struct vm_area_struct * vma)
{
    // increment ref counter of pa_struct
    atomic_inc(&(((struct pa_state_struct *)(vma->vm_private_data))->ref_count));
    printk(KERN_INFO "paging_vma_open() invoked\n");
}

static void
paging_vma_close(struct vm_area_struct * vma)
{
    // decrement ref counter, if 0 release mem
    atomic_dec(&(((struct pa_state_struct *)(vma->vm_private_data))->ref_count));

    if(atomic_read(&((struct pa_state_struct *)(vma->vm_private_data))->ref_count) == 0) {
        // iterate and free llist's ref pages, delete node, free node struct
        struct ref_page_node *page_node, *temp;
        list_for_each_entry_safe(page_node, temp, &ref_page_list, list) {
            __free_page(page_node->page);
            list_del(&(page_node->list));
            kfree(page_node);
            atomic_inc(&free_count);    // increment page free counter
        }
        // free pa_state_struct
        kfree(vma->vm_private_data);
    }

    printk(KERN_INFO "paging_vma_close() invoked\n");
}

static struct vm_operations_struct
paging_vma_ops = 
{
    .fault = paging_vma_fault,
    .open  = paging_vma_open,
    .close = paging_vma_close
};

/* vma is the new virtual address segment for the process */
static int
paging_mmap(struct file           * filp,
            struct vm_area_struct * vma)
{
    struct pa_state_struct * pas;

    printk(KERN_INFO "paging_mmap() invoked: new VMA for pid %d from VA 0x%lx to 0x%lx\n",
        current->pid, vma->vm_start, vma->vm_end);

    /* prevent Linux from mucking with our VMA (expanding it, merging it 
     * with other VMAs, etc.)
     */
    vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_NORESERVE
              | VM_DONTDUMP | VM_PFNMAP;

    /* setup the vma->vm_ops, so we can catch page faults */
    vma->vm_ops = &paging_vma_ops;
    
    // dynamically alloc pa_state_struct and store in vma->vm_private_data
    pas = kmalloc(sizeof(struct pa_state_struct), GFP_KERNEL);
    if (!pas) {
      // could not alloc struct
      printk(KERN_ALERT "could not allocate memeory for pa_state_struct!\n");
      return ENOMEM;
    }

    // set pas initial ref count to 1
    atomic_set(&(pas->ref_count), 1);

    vma->vm_private_data = pas;
    
    // pre-paging 
    printk(KERN_INFO "is demand_paging: %d\n", demand_paging);
    if (!demand_paging) {
        // for loop entire vma address range, call do_fault, and return failures within loop, return mapping if successful
        unsigned long addr;
        int result;
        printk(KERN_INFO "vm_start: 0x%lx   vm_end: 0x%lx   PAGE_SIZE: 0x%lx\n", vma->vm_start, vma->vm_end, PAGE_SIZE);
        for (addr = vma->vm_start; addr < vma->vm_end; addr += PAGE_SIZE) {
            result = do_fault(vma, addr);
            if (result == VM_FAULT_OOM) {
                printk(KERN_ALERT "Page allocation error\n");
                return result;
            }
            else if (result == VM_FAULT_SIGBUS) {
                printk(KERN_ALERT "Sigbus error\n");
                return result;
            }
        } 
    }

    return 0;
}

static struct file_operations
dev_ops =
{
    .mmap = paging_mmap,
};

static struct miscdevice
dev_handle =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = PAGING_MODULE_NAME,
    .fops = &dev_ops,
};
/*** END device I/O **/

/*** Kernel module initialization and teardown ***/
static int
kmod_paging_init(void)
{
    int status;
    
    // set atomic alloc_counter and free_counter to 0
    atomic_set(&alloc_count, 0);
    atomic_set(&free_count, 0);

    /* Create a character device to communicate with user-space via file I/O operations */
    status = misc_register(&dev_handle);
    if (status != 0) {
        printk(KERN_ERR "Failed to register misc. device for module\n");
        return status;
    }

    printk(KERN_INFO "Loaded kmod_paging module\n");

    return 0;
}

static void
kmod_paging_exit(void)
{
    /* Deregister our device file */
    misc_deregister(&dev_handle);

    printk(KERN_INFO "Unloaded kmod_paging module\n");
    printk(KERN_INFO "Verify memory cleanup: %d allocs with %d frees\n", atomic_read(&alloc_count), atomic_read(&free_count));
}

module_init(kmod_paging_init);
module_exit(kmod_paging_exit);

/* Misc module info */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("breadnmkii");
MODULE_DESCRIPTION("Module for PA management of VMA called by proc");
