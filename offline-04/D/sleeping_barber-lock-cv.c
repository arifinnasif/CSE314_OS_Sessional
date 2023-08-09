#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "zemaphore.h"

#define TOTAL_CHAIRS 5
#define MIN_TIME_BETWEEN_CUSTOMERS_MICROSECONDS 10
#define VAR_TIME_BETWEEN_CUSTOMERS_MICROSECONDS 100
#define TIME_FOR_HAIRCUT_MICROSECONDS 1000
#define MAX_CUSTOMERS 100

pthread_mutex_t mutex;
pthread_mutex_t customer_ready_mutex;
pthread_mutex_t barber_ready_mutex;

pthread_cond_t barber_ready;
int barber_ready_int=0;
pthread_cond_t customer_ready;
int customer_ready_int=0;
pthread_mutex_t access_wr;

pthread_mutex_t customers_left_mutex;

int empty_chairs=TOTAL_CHAIRS;
int customers_served=0;
int customers_left=0;
int total_customers=MAX_CUSTOMERS;

void *barber(void *arg) {
  int barber_id = *((int *) arg);

//   int counter = 0;

  while(1) {
    // zem_down(&customer_ready);
    pthread_mutex_lock(&customer_ready_mutex);
    while(customer_ready_int <= 0) {
        pthread_cond_wait(&customer_ready, &customer_ready_mutex);
    }
    customer_ready_int--;
    pthread_mutex_unlock(&customer_ready_mutex);

    // zem_down(&access_wr);
    pthread_mutex_lock(&access_wr);

    
    empty_chairs++;
    // printf("Barber is ready to serve a customer\n");
    // printf("Number of empty chairs: %d\n", empty_chairs);

    // zem_up(&access_wr);
    pthread_mutex_unlock(&access_wr);

    // zem_up(&barber_ready);
    pthread_mutex_lock(&barber_ready_mutex);
    barber_ready_int++;
    pthread_cond_signal(&barber_ready);
    pthread_mutex_unlock(&barber_ready_mutex);

    // zem_down(&mutex_zem);
    pthread_mutex_lock(&mutex);

    // service
    printf("Barber is cutting hair\n");
    usleep(TIME_FOR_HAIRCUT_MICROSECONDS);
    
    // zem_up(&mutex_zem);
    pthread_mutex_unlock(&mutex);

    printf("Barber has served a customer\n");

    // counter++;
    customers_served++;

    pthread_mutex_lock(&customers_left_mutex);
    if(customers_served + customers_left == total_customers) {
      pthread_mutex_unlock(&customers_left_mutex);
      break;
    }
    pthread_mutex_unlock(&customers_left_mutex);
  }
}

void *customer(void *arg) {
    int customer_id = *((int *) arg);
    
    // zem_down(&access_wr);
    pthread_mutex_lock(&access_wr);
    
    if(empty_chairs > 0) {
        empty_chairs--;
        printf("Customer %d is waiting\n", customer_id);
        printf("Number of empty chairs: %d\n", empty_chairs);
        // zem_up(&customer_ready);
        pthread_mutex_lock(&customer_ready_mutex);
        customer_ready_int++;
        pthread_cond_signal(&customer_ready);
        pthread_mutex_unlock(&customer_ready_mutex);

        // zem_up(&access_wr);
        pthread_mutex_unlock(&access_wr);

        // zem_down(&barber_ready);
        pthread_mutex_lock(&barber_ready_mutex);
        while(barber_ready_int <= 0) {
            pthread_cond_wait(&barber_ready, &barber_ready_mutex);
        }
        barber_ready_int--;
        pthread_mutex_unlock(&barber_ready_mutex);
        
        printf("Customer %d is getting a haircut\n", customer_id);
        
        
    } else {
        // zem_up(&access_wr);
        pthread_mutex_unlock(&access_wr);

        // leave
        pthread_mutex_lock(&customers_left_mutex);
        customers_left++;
        pthread_mutex_unlock(&customers_left_mutex);
        printf("Customer %d is leaving\n", customer_id);
    }
}


int main() {
    srand(5097);
    pthread_t customer_threads[MAX_CUSTOMERS];
    pthread_t barber_thread;
    
    int customer_ids[MAX_CUSTOMERS];
    int barber_ids[1];

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&customer_ready_mutex, NULL);
    pthread_mutex_init(&barber_ready_mutex, NULL);
    pthread_mutex_init(&access_wr, NULL);
    pthread_mutex_init(&customers_left_mutex, NULL);
    pthread_cond_init(&customer_ready, NULL);
    pthread_cond_init(&barber_ready, NULL);
    

    

    barber_ids[0] = 0;
    pthread_create(&barber_thread, NULL, barber, &barber_ids[0]);
    
    for(int i = 0; i < MAX_CUSTOMERS; i++) {
        customer_ids[i] = i;
        pthread_create(&customer_threads[i], NULL, customer, &customer_ids[i]);
        usleep(MIN_TIME_BETWEEN_CUSTOMERS_MICROSECONDS + rand() % VAR_TIME_BETWEEN_CUSTOMERS_MICROSECONDS);
    }

    

    pthread_join(barber_thread, NULL);
    
    for(int i = 0; i < MAX_CUSTOMERS; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    return 0;
}