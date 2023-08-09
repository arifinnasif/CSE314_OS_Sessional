#include "rwlock.h"

void InitalizeReadWriteLock(rwlock_t * rw)
{
  //	Write the code for initializing your read-write lock.
  rw->readers = 0;
  rw->writers = 0;

  rw->currentReaders = 0;

  rw->readerMode = 0;
  rw->writerMode = 0;

  pthread_mutex_init(&rw->lock, NULL);
  pthread_cond_init(&rw->read, NULL);
  pthread_cond_init(&rw->write, NULL);
}

void ReaderLock(rwlock_t * rw){
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&rw->lock);

  printf("reader came\n");
  rw->readers++;

  while( rw->writers > 0 ){
    printf("- reader waiting\n");
    pthread_cond_wait(&rw->read, &rw->lock);
  }

  printf("- reader got lock - now current reader\n");
  rw->readerMode = 1;
  rw->currentReaders++;

  pthread_mutex_unlock(&rw->lock);
}

void ReaderUnlock(rwlock_t * rw){
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&rw->lock);

  printf("current reader left\n");
  rw->readers--;
  rw->currentReaders--;

  if( rw->writers > 0 && rw->currentReaders == 0 ){
    printf("- last current reader left\n");
    rw->readerMode = 0;
    pthread_cond_signal(&rw->write);
  }

  pthread_mutex_unlock(&rw->lock);
}

void WriterLock(rwlock_t * rw){
  //	Write the code for aquiring read-write lock by the writer.
  pthread_mutex_lock(&rw->lock);

  printf("writer came\n");
  rw->writers++;

  while( rw->writerMode ){
    printf("- writer waiting for other writer to finish\n");
    pthread_cond_wait(&rw->write, &rw->lock);
  }

  while( rw->readerMode && rw->currentReaders > 0 ){
    printf("- writer waiting for current readers to finish\n");
    pthread_cond_wait(&rw->write, &rw->lock);
  }

  printf("- writer got lock\n"); 
  rw->writerMode = 1;

  pthread_mutex_unlock(&rw->lock);
}

void WriterUnlock(rwlock_t * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  pthread_mutex_lock(&rw->lock);

  printf("writer left\n");
  rw->writers--;
  rw->writerMode = 0;

  if( rw->writers > 0 ){
    printf("- there are still writers waiting - signaling them\n");
    pthread_cond_signal(&rw->write);
  }
  else if( rw->readers > 0 ){
    printf("- there are no writers - but still readers waiting\n");
    pthread_cond_broadcast(&rw->read);
  }

  pthread_mutex_unlock(&rw->lock);
}
