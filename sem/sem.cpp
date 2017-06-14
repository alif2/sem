#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stack>
#include <stdio.h>
#include <unistd.h>

sem_t mutex, empty, full;
std::stack<char> buffer;
int items;

void *producer(void *args) {
    char push = 'X';
    unsigned long id = pthread_self();
    
    while(1) {
        sem_wait(&empty);
        
        if(items < 1) {
            sem_post(&empty);
            return NULL;
        }
        
        sem_wait(&mutex);
 
        buffer.push(push);
        items--;
        
        sem_post(&mutex);
        sem_post(&full);
        
        // Item index always 0 due to stack implementation
        printf("p:<%lu>, item: %c, at %d\n", id, push, 0);
    }
}

void *consumer(void *args) {
    unsigned long id = pthread_self();
    
    while(1) {
        if(buffer.size() < 1 && items < 1) return NULL;
        
        sem_wait(&full);
        sem_wait(&mutex);
        
        char pop = buffer.top();
        buffer.pop();
        
        sem_post(&mutex);
        sem_post(&empty);
        
        // Item index always 0 due to stack implementation
        printf("c:<%lu>, item: %c, at %d\n", id, pop, 0);
    }
}

int main(int argc, char** argv) {
    int i, bufl, pt, ct;
    
    while((i = getopt(argc, argv, ":b:c:i:p:")) != -1) {
        switch(i) {
            case 'b':                
                bufl = atoi(optarg);
                break;
            case 'c':
                ct = atoi(optarg);
                break;
            case 'i':
                items = atoi(optarg);
                break;
            case 'p':
                pt = atoi(optarg);
                break;
            default:
                printf("Error: Incorrect input\n");
                exit(0);
        }
    }
    
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, bufl);
    sem_init(&full, 0, 0);
    
    pthread_t produce[pt];
    pthread_t consume[ct];
    
    for(int j = 0; j < pt; j++) {
        int e = pthread_create(&produce[j], NULL, producer, (void*) NULL);
        
        if(e) {
            printf("Failed to create producer %i\n", j);
        }
    }
    
    for(int j = 0; j < ct; j++) {
        int e = pthread_create(&consume[j], NULL, consumer, (void*) NULL);
        
        if(e) {
            printf("Failed to create consumer %i\n", j);
        }
    }

    // Wait for input to complete
    while(items > 0);
    while(buffer.size() > 0);
    
    printf("Complete. Press Enter to quit\n");
    std::cin.get();
    return 0;
}