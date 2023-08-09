#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

#include "zemaphore.h"

#define NUM_THREADS 26
#define NUM_ITER 10

zem_t zemaphores[NUM_THREADS];

void *justprint(void *data)
{
  int thread_id = *((int *)data);

  // for(int i=0; i < NUM_ITER; i++)
  while(1)
    {
      zem_down(&zemaphores[thread_id]);
      // printf("This is thread %c\n", 'a'+25-thread_id);
      printf("%c\n", 'a'+thread_id);
      if(thread_id!=0)
        zem_up(&zemaphores[(thread_id-1)%NUM_THREADS]);
      else
        zem_up(&zemaphores[25]);
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  
  zem_init(&zemaphores[25], 1);
  for(int i =0; i < NUM_THREADS-1; i++)
    {
      zem_init(&zemaphores[i], 0);
    }

  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
