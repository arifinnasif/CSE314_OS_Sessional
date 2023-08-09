#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "zemaphore.h"

#define TOTAL_CHAIRS 5
#define MIN_TIME_BETWEEN_CUSTOMERS_MICROSECONDS 1000
#define VAR_TIME_BETWEEN_CUSTOMERS_MICROSECONDS 10
#define TIME_FOR_HAIRCUT_MICROSECONDS 10
#define MAX_CUSTOMERS 100

zem_t mutex_zem;

zem_t barber_ready;
zem_t customer_ready;
zem_t access_wr;
zem_t customers_left_mutex;

int empty_chairs=TOTAL_CHAIRS;
int customers_served=0;
int customers_left=0;
int total_customers=MAX_CUSTOMERS;

void *barber(void *arg) {
  int barber_id = *((int *) arg);

//   int counter = 0;

  while(1) {
    zem_down(&customer_ready);
    zem_down(&access_wr);
    
    empty_chairs++;
    // printf("Barber is ready to serve a customer\n");
    // printf("Number of empty chairs: %d\n", empty_chairs);

    zem_up(&access_wr);

    zem_up(&barber_ready);

    zem_down(&mutex_zem);
    // service
    printf("Barber is cutting hair\n");
    usleep(TIME_FOR_HAIRCUT_MICROSECONDS);
    zem_up(&mutex_zem);

    printf("Barber has served a customer\n");

    // counter++;
    customers_served++;

    zem_down(&customers_left_mutex);
    if(customers_served + customers_left == total_customers) {
        zem_up(&customers_left_mutex);
        break;
      
    }
    zem_up(&customers_left_mutex);
  }
}

void *customer(void *arg) {
    int customer_id = *((int *) arg);
    
    zem_down(&access_wr);
    
    if(empty_chairs > 0) {
        empty_chairs--;
        printf("Customer %d is waiting\n", customer_id);
        printf("Number of empty chairs: %d\n", empty_chairs);
        zem_up(&customer_ready);
        zem_up(&access_wr);
        zem_down(&barber_ready);
        
        printf("Customer %d is getting a haircut\n", customer_id);
        
        
    } else {
        zem_up(&access_wr);
        // leave
        zem_down(&customers_left_mutex);
        customers_left++;
        zem_up(&customers_left_mutex);
        printf("Customer %d is leaving\n", customer_id);
    }
}


int main() {
    srand(5097);
    pthread_t customer_threads[MAX_CUSTOMERS];
    pthread_t barber_thread;
    
    int customer_ids[MAX_CUSTOMERS];
    int barber_ids[1];
    

    zem_init(&mutex_zem, 1);
    zem_init(&barber_ready, 0);
    zem_init(&customer_ready, 0);
    zem_init(&access_wr, 1);
    zem_init(&customers_left_mutex, 1);

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