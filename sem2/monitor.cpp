#include <semaphore.h>
#include <stack>
#include "monitor.h"

using namespace std;

int count, bmax;
stack<char> buffer;
sem_t empty, full, mutex;

monitor::monitor() {
    sem_init(&empty, 0, 10);
    sem_init(&full, 0, 0);
    
    count = 0;
    bmax = 25;
}

monitor::monitor(int size) {
    sem_init(&empty, 0, size);
    sem_init(&full, 0, 0);
    
    count = 0;
    bmax = size;
}

void monitor::mon_insert(char alpha) {
    while(count == bmax) sem_wait(&full);
    sem_wait(&mutex);
    
    buffer.push(alpha);
    count++;
    
    sem_post(&mutex);
    sem_post(&empty);
}

char monitor::mon_remove() {
    while(count < 1) sem_wait(&empty);
    sem_wait(&mutex);
    
    char result = buffer.top();
    buffer.pop();
    count--;
    
    sem_post(&mutex);
    sem_post(&full);
    return result;
}

int monitor::size() {
    return buffer.size();
}