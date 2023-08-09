#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "zemaphore.h"

void zem_init(zem_t *s, int value) {
    s->n = value;
    if(pthread_cond_init(&s->cond, NULL) != 0) {
        perror("error at \"pthread_cond_init\" in \"zem_init\"");
        exit(1);
    }
    if(pthread_mutex_init(&s->lock, NULL) != 0) {
        perror("error at \"pthread_mutex_init\" in \"zem_init\"");
        exit(1);
    }
}

void zem_down(zem_t *s) {
    if(pthread_mutex_lock(&s->lock) != 0) {
        perror("error at \"pthread_mutex_lock\" in \"zem_down\"");
        exit(1);
    }
    // s->n--;
    while(s->n <= 0) {
        if(pthread_cond_wait(&s->cond, &s->lock) != 0) {
            perror("error at \"pthread_cond_wait\" in \"zem_down\"");
            exit(1);
        }
    }
    s->n--;

    if(pthread_mutex_unlock(&s->lock) != 0) {
        perror("error at \"pthread_mutex_unlock\" in \"zem_down\"");
        exit(1);
    }
}

void zem_up(zem_t *s) {
    if(pthread_mutex_lock(&s->lock) != 0) {
        perror("error at \"pthread_mutex_lock\" in \"zem_up\"");
        exit(1);
    }
    s->n++;
    if(pthread_cond_signal(&s->cond) != 0) {
        perror("error at \"pthread_cond_signal\" in \"zem_up\"");
        exit(1);
    }
    if(pthread_mutex_unlock(&s->lock) != 0) {
        perror("error at \"pthread_mutex_unlock\" in \"zem_up\"");
        exit(1);
    }
}
