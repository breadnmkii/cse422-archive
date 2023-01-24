#define NAME "/sharedobj"
#define SHARE_SIZE 2000000

typedef struct shared_data {
  volatile int write_guard;
  volatile int read_guard;
  volatile int delete_guard;
  volatile int data[SHARE_SIZE];
} shared_data;

