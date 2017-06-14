#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stack>
#include <stdio.h>
#include <unistd.h>

#include "monitor.h"

int items;
monitor *mon;

unsigned char randChar() {
    srand(time(NULL) + rand());
    
    int i = rand() % 57 + 65;
    while(i > 90 && i < 97) {
        i = rand() % 57 + 65;
    }
    
    return (unsigned char) i;
}

void *producer(void *args) {
    unsigned long id = pthread_self();

    while(1 && items > 0) {
        char c = randChar();
        items--;
        
        mon->mon_insert(c);
        printf("p:<%lu>, item: %c, at %d\n", id, c, 0);
    }
}

void *consumer(void *args) {
    unsigned long id = pthread_self();

    while(1) {
        if(items < 1 && mon->size() < 1) return NULL;
        
        char c = mon->mon_remove();
        printf("c:<%lu>, item: %c, at %d\n", id, c, 0);
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

    mon = new monitor(bufl);
    
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
    while(mon->size() > 0);

    printf("Complete. Press Enter to quit\n");
    std::cin.get();
    
    delete(mon);
    return 0;
}