#include <semaphore.h>
#include <stack>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include "monitor.h"

#include <stdio.h>

int count, bmax;
std::stack<char> buffer;
//sem_t empty, full, mutex;

struct cond {
    sem_t lock;
    std::queue<unsigned long> waiting;
} empty, full, mutex;

void cond_init(cond *c, int value) {
    sem_init(&c->lock, 0, value);
}

unsigned long cond_count(cond *c) {
    return c->waiting.size();
}

void cond_wait(cond *c) {
    unsigned long id = pthread_self();
    c->waiting.push(id);
    while(id != c->waiting.front());
}

void cond_signal(cond *c) {
    c->waiting.pop();
}

monitor::monitor(int size) {
    //sem_init(&empty, 0, size);
    //sem_init(&full, 0, 0);
    //sem_init(&mutex,0, 1);
    cond_init(&empty, size);
    cond_init(&full, 0);
    cond_init(&mutex, 1);
    count = 0;
    bmax = size;
}

void monitor::mon_insert(char alpha) {
    while(count >= bmax) cond_wait(&full);//sem_wait(&full);
    //sem_wait(&mutex);
    cond_wait(&mutex);
    buffer.push(alpha);
    count++;
    cond_signal(&mutex);
    cond_signal(&empty);
    //sem_post(&mutex);
    //sem_post(&empty);
}

char monitor::mon_remove() {
    while(count < 1) cond_wait(&empty);//sem_wait(&empty);
    //sem_wait(&mutex);
    cond_wait(&mutex);
    char result = buffer.top();
    buffer.pop();
    count--;
    cond_signal(&mutex);
    cond_signal(&full);
    //sem_post(&mutex);
    //sem_post(&full);
    return result;
}

int monitor::size() {
    return buffer.size();
}