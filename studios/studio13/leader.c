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
  int shm_fd = shm_open("/sharedobj", O_RDWR | O_CREAT, S_IRWXU);
  ftruncate(shm_fd, sizeof(shared_data));
  void * mapping = mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (mapping < 0) {
    perror("mmap() error: ");
    return -1;
  }
  
  // map shared struct to mamp memory
  data = (shared_data *) mapping;
  
  // wait for follower to be created
  printf("Waiting for follower to create...\n");
  while (data->write_guard == 0) {}
  printf("Follower created, writing to shared data...\n");

  // populate local array
  for (i=0; i<SHARE_SIZE; ++i) {
    arr[i] = rand();
  }

  // copy array to shared data
  for (i=0; i<SHARE_SIZE; ++i) {
    data->data[i] = arr[i];
  }

  // notify follower to read
  printf("Notified follower to read, waiting for acknowledge...\n");
  data->read_guard = 1;
  while(data->delete_guard == 0) {}

  // delete shared data
  printf("Follower completed read, deleting data...\n");
  close(shm_fd);

  return 0;
}
