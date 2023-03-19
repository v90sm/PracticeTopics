
/**
LC 1188: design-bounded-blocking-queue
https://leetcode.com/problems/design-bounded-blocking-queue/

Using semaphore and mutex only.
Notes: This solution is still in discussion..
*/

#include <stdio.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
sem_t empty;
sem_t full;
int write_index = 0;
int read_index = 0;


/**
 * For interrupt handler we cannot use semaphore wait or mutex locks. This sol is not good for interrupt.
 * 
 * SP MC: NO mutex for writer, mutex needed for reader (Interrupt(cannot have locks) is producer, multi consumer threads)
 * MP SC: mutex needed for writer, NOT needed for consumer/reader 
 * MP MC: mutex needed for writer, mutex needed for reader
 * 
*/
void add_element(int element) {
    sem_wait(&empty);
    mutex_wr.lock(); // wr mutex
    buffer[write_index] = element;
    write_index = (write_index + 1) % BUFFER_SIZE;
    mutex_wr.unlock();
    sem_post(&full);
}

int remove_element() {
    sem_wait(&full);
    mutex_rd.lock();  //rd mutex
    int element = buffer[read_index];
    read_index = (read_index + 1) % BUFFER_SIZE;
    mutex_rd.lock();
    sem_post(&empty);
    return element;
}

int main() {
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    // Add elements to buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        add_element(i);
        printf("Added element %d to buffer\n", i);
    }

    // Remove elements from buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        int element = remove_element();
        printf("Removed element %d from buffer\n", element);
    }

    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
