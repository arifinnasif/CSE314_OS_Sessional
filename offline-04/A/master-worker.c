#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>


int total_items, max_buf_size, num_workers, num_masters;

int item_to_produce, total_item_consumed, curr_buf_size;

int *buffer;
pthread_mutex_t lock;
pthread_cond_t empty;
pthread_cond_t fill;


void print_produced(int num, int master){
  printf("Produced %d by master %d\n", num, master);
}
void print_consumed(int num, int worker){
  printf("Consumed %d by worker %d\n", num, worker);
}


void *produce(void *_thread_id);

void *consume(void *_thread_id);


int main(int argc, char *argv[]){

    item_to_produce = 0;
    total_item_consumed = 0;
    curr_buf_size = 0;

    int *master_thread_id;
    pthread_t *master_thread;

    int *worker_thread_id;
    pthread_t *worker_thread;


    //-----------------------------input--------------------------
    // total_items = 5;
    // max_buf_size = 2;
    // num_workers = 3;
    // num_masters = 2;

    if (argc < 5)    {
        printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
        exit(1);
    } else {
        total_items = atoi(argv[1]);
        max_buf_size = atoi(argv[2]);
        num_workers = atoi(argv[3]);
        num_masters = atoi(argv[4]);
    }


    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&fill, NULL);


    buffer = (int*) malloc(sizeof(int) * max_buf_size);

    //-------------create master threads----------------
    master_thread_id = (int *)malloc(sizeof(int) * num_masters);
    master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);

    for( int i = 0; i < num_masters; i++){
        master_thread_id[i] = i;
        pthread_create(&master_thread[i], NULL, produce, (void *)&master_thread_id[i]);
    }

    //-------------create worker threads----------------
    worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
    worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);

    for( int i = 0; i < num_workers; i++ ){
        worker_thread_id[i] = i;
        pthread_create(&worker_thread[i], NULL, consume, (void *)&worker_thread_id[i]);
    }


    //-------------join master threads----------------
    for( int i = 0; i < num_masters; i++){
        pthread_join(master_thread[i], NULL);
        printf("master %d joined\n", i);
    }

    //-------------join worker threads----------------
    for( int i = 0; i < num_workers; i++ ){
        pthread_join(worker_thread[i], NULL);
        printf("worker %d joined\n", i);
    }



    //--------------Deallocating Buffers---------------------
    free(buffer);
    free(master_thread_id);
    free(master_thread);
    free(worker_thread_id);
    free(worker_thread);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&fill);  

    return 0;
  
}




void *produce(void *_thread_id){
    int thread_id =  *((int*) _thread_id);

    //printf("executing by master %d\n", thread_id);

    while(1){
        pthread_mutex_lock(&lock);

        //printf("\tbefore while : master %d curr_buf_size = %d -- item_to_produce = %d\n", thread_id, curr_buf_size, item_to_produce);

        while( curr_buf_size >= max_buf_size ){
            //printf("\tin while : master %d curr_buf_size = %d -- item_to_produce = %d\n", thread_id, curr_buf_size, item_to_produce);
            //printf("master %d waiting\n", thread_id);
            pthread_cond_wait(&empty, &lock);
        }


        //printf("\tafter while : master %d curr_buf_size = %d -- item_to_produce = %d\n", thread_id, curr_buf_size, item_to_produce);  

        if( item_to_produce >= total_items ){
            //printf("master %d exiting\n", thread_id);

            pthread_cond_broadcast(&fill);
            //pthread_cond_broadcast(&empty);

            pthread_mutex_unlock(&lock);
            break;
        }

        buffer[curr_buf_size++] = item_to_produce;
        print_produced(item_to_produce, thread_id);

        item_to_produce++;

        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&lock);
    }
}

void *consume(void *_thread_id){
    int thread_id =  *((int*) _thread_id);

    //printf("executing by worker %d\n", thread_id);

    while(1){
        pthread_mutex_lock(&lock);

        //printf("\tbefore while : worker %d curr_buf_size = %d -- total_item_consumed = %d\n", thread_id, curr_buf_size, total_item_consumed);

        while( curr_buf_size == 0 && total_item_consumed < total_items ){
            //printf("\tbefore while : worker %d curr_buf_size = %d -- total_item_consumed = %d\n", thread_id, curr_buf_size, total_item_consumed);
            //printf("worker %d waiting\n", thread_id);
            pthread_cond_wait(&fill, &lock);
        }
            
        //printf("\tafter while : worker %d curr_buf_size = %d -- total_item_consumed = %d\n", thread_id, curr_buf_size, total_item_consumed);

        if( total_item_consumed == total_items ){
            //printf("worker %d exiting\n", thread_id);

            pthread_cond_broadcast(&fill);
            pthread_cond_broadcast(&empty);

            pthread_mutex_unlock(&lock);
            break;
        }

        int item = buffer[--curr_buf_size];
        print_consumed(item, thread_id);

        total_item_consumed++;

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&lock);
    }
}