#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "shared_process.h"

struct shared_data * data;
volatile int arr[SHARE_SIZE];


int main() {
  int i;
  int shm_fd = shm_open("/sharedobj", O_RDWR, S_IRWXU);
  void * mapping = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (mapping < 0) {
    perror("mmap() error: ");
    return -1;
  }
  
  // map shared struct to mamp memory
  data = (shared_data *) mapping;
  
  // notify leader creation, wait for write
  data->write_guard = 1;
  printf("Notified leader on create, waiting for write...\n");
  while (data->read_guard == 0) {}
  printf("Leader completed write, Reading data...\n");
  
  // populate local array
  memcpy((void *)arr, (void *)data + 12, SHARE_SIZE * sizeof(volatile int));

  // print local array
/*
  printf("local array:"); 
  for (i=0; i<SHARE_SIZE; ++i) {
    printf(" %d ", arr[i]);
  }
  printf("\n");
*/
  // notify leader completed read, unlinking shared data
  shm_unlink("/sharedobj");
  data->delete_guard = 1;

  return 0;

}
