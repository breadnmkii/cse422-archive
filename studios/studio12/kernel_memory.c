#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/types.h>

#include <asm/uaccess.h>

#define ARR_SIZE 8
typedef struct datatype_t {
	unsigned int array[ARR_SIZE];
} datatype;

static uint nr_structs = 2000;
module_param(nr_structs, uint, 0644); 

static struct task_struct * kthread = NULL;

// Step 3 modification
static unsigned int nr_pages; // num of pages required for holding num of datatype structs
static unsigned int order;    // log_2 order of pages to be allocd from nr_pages
static unsigned int nr_structs_per_page; // num of datatypes structs per page

// Step 4 pointer
static struct page * pages;

const int STRUCT_SIZE = sizeof(datatype);

static unsigned int
my_get_order(unsigned int value)
{
    unsigned int shifts = 0;
    // if is an empty value
    if (!value)
        return 0;
    // if is a perfect power of 2
    if (!(value & (value - 1)))
        value--;
    // continuous right shifts to determine num of powers of 2 that can be divided from value
    while (value > 0) {
        value >>= 1;
        shifts++;
    }

    return shifts;
}

static int
thread_fn(void * data)
{
    static struct datatype_t * this_struct;    
    unsigned long pages_phyAddr;
    int i, j, k;

    // perform calculations
    nr_structs_per_page = (int)(PAGE_SIZE/STRUCT_SIZE);
    nr_pages = (int)(nr_structs/nr_structs_per_page);
    // if remainder exists from modulus, alloc additional page
    if (nr_structs%nr_structs_per_page) {
        ++nr_pages;
    }
    order = my_get_order(nr_pages);
 
    printk("Hello from thread %s. nr_structs=%u\n", current->comm, nr_structs);

    printk("Page size: %lu\n", PAGE_SIZE);
    printk("Size of datatype: %u\n", STRUCT_SIZE);
    printk("Structs per page: %u\n", nr_structs_per_page);
    printk("Number of pages: %u\n", nr_pages);
    printk("Order: %u\n", order);

    // perform alloc
    if (!(pages = alloc_pages(GFP_KERNEL, order))) {
        printk(KERN_WARNING "Could not allocate pages!\n");
    }

    // get physical addr of alloc mem
    pages_phyAddr = PFN_PHYS(page_to_pfn(pages));
    
    // perform translations
    for (i=0; i<nr_pages; ++i) {
        unsigned long curr_page = pages_phyAddr + i*PAGE_SIZE;
        
        for (j=0; j<nr_structs_per_page; ++j) {
            unsigned long curr_struct = curr_page + j*STRUCT_SIZE;   
            this_struct = (struct datatype_t *)__va(curr_struct);
            // set numbers
            //printk("Writing to struct arrays...\n");
            for (k=0; k<ARR_SIZE; ++k) {
                if (this_struct) {
                    this_struct->array[k] = i*nr_structs_per_page*ARR_SIZE + j*ARR_SIZE + k;
                } else {
                   printk(KERN_ALERT "Struct is null!\n");
                }
                //printk(KERN_INFO "page:%d  j:%d  k:%d\n", i, j, k);
            }
        }
    }
    
    while (!kthread_should_stop()) {
        schedule();
    }

    return 0;
}

static int
kernel_memory_init(void)
{
    printk(KERN_INFO "Loaded kernel_memory module\n");

    kthread = kthread_create(thread_fn, NULL, "k_memory");
    if (IS_ERR(kthread)) {
        printk(KERN_ERR "Failed to create kernel thread\n");
        return PTR_ERR(kthread);
    }
    
    wake_up_process(kthread);

    return 0;
}

static void 
kernel_memory_exit(void)
{
    static struct datatype_t * this_struct;
    int cleanData = 1;
    int i, j, k;
    unsigned long pages_phyAddr;

    kthread_stop(kthread);

    pages_phyAddr = PFN_PHYS(page_to_pfn(pages));

    // checking data values
    for (i=0; i<nr_pages; ++i) {
        unsigned long curr_page = pages_phyAddr + i*PAGE_SIZE;
        
        for (j=0; j<nr_structs_per_page; ++j) {
            unsigned long curr_struct = curr_page + j*STRUCT_SIZE;   
            this_struct = (struct datatype_t *)__va(curr_struct);
            // check numbers
            for (k=0; k<ARR_SIZE; ++k) {
                if (this_struct->array[k] != i*nr_structs_per_page*ARR_SIZE + j*ARR_SIZE + k) {
                    printk(KERN_ALERT "Data at page %d, struct %d, index %d is corrupt!\n", i, j, k);
                    cleanData = 0;
                }
            }
        }
    }
    // free allocated pages
    __free_pages(pages, order);

    if (cleanData) {
        printk(KERN_INFO "Pages were clean\n");
    } else {
        printk(KERN_INFO "Pages were not clean\n");
    }

    printk(KERN_INFO "Unloaded kernel_memory module\n");
}

module_init(kernel_memory_init);
module_exit(kernel_memory_exit);

MODULE_LICENSE ("GPL");
