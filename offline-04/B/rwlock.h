#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

typedef struct read_write_lock {
    pthread_mutex_t lock;

    pthread_cond_t read;
    pthread_cond_t write;

    int readerMode;
    int writerMode;

    int readers;
    int writers;

    int currentReaders;

} rwlock_t;

void InitalizeReadWriteLock(rwlock_t * rw);
void ReaderLock(rwlock_t * rw);
void ReaderUnlock(rwlock_t * rw);
void WriterLock(rwlock_t * rw);
void WriterUnlock(rwlock_t * rw);
