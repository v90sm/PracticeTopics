/**
   Reader index modified by reader ONLY, write index modified by writer ONLY.
   separate condition variable for isFull and isEmpty.
   In this version, there are two separate mutexes for the reader and writer threads,
   which allows them to run concurrently.
*/

#include <pthread.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE +1];
    pthread_mutex_t reader_mutex;
    pthread_mutex_t writer_mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    size_t write_index;
    size_t read_index;
} CircularBuffer;

void circular_buffer_init(CircularBuffer *circular_buffer) {
    circular_buffer->write_index = 0;
    circular_buffer->read_index = 0;
    pthread_mutex_init(&circular_buffer->reader_mutex, NULL);
    pthread_mutex_init(&circular_buffer->writer_mutex, NULL);
    pthread_cond_init(&circular_buffer->not_full, NULL);
    pthread_cond_init(&circular_buffer->not_empty, NULL);
}

/**
 * size = 3
 * rd=0 wr=0
 * en(6) -> a[0] = 6  wr= 1
 * en(7) -> a[1] = 7  wr= 2
 * en(8) -> a[x]     wr = 2+ 1 = 3%3 = 0 = rd  ( q is full)  / th1
 * en(9) -> a[]      wr = 2+ 1 = 3%3 = 0 = rd  ( q is full)  / th2
 * dq    -> rd=0  wr = 2   ans = a[0] = 6 rd=1   wr=2
 * th1 (th1 or th2 based on scheduling mechanism) wakes up, acquires writer_mutex, then since its a while loop, it check line 40)
 * wr= 2 +1 % 3= 0   is( 0 == 1?) NO, so Q is not full, While breaks
 *           a[2]  = 8;  wr= 0;  rd=1;
 * write mutex unlock, th2 wakes up - acquires writer lock, will exit the pthread_cond_wait, locks the mutex and do a while check
 *  en(9) goes back in wait state becoz   ( wr+ 1 = rd), 0+1 = 1 and releases the mutex
 * 
 * dq     rd was 1, wr=0, not EQ, buff not empty:     ans = a[1] = 7, rd = 1+1 = 2 , wr=0;
 * suppose th2 is awaken en(9)  A[0] = 9  wr = 1, rd =2
 * 
*/

void circular_buffer_add_element(CircularBuffer *circular_buffer, int element) {
    pthread_mutex_lock(&circular_buffer->writer_mutex);
    while (((circular_buffer->write_index + 1) % BUFFER_SIZE) == circular_buffer->read_index) {
        pthread_cond_wait(&circular_buffer->not_full, &circular_buffer->writer_mutex);
    }
    circular_buffer->buffer[circular_buffer->write_index] = element;
    circular_buffer->write_index = (circular_buffer->write_index + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&circular_buffer->writer_mutex);
    pthread_cond_signal(&circular_buffer->not_empty);
}

int circular_buffer_remove_element(CircularBuffer *circular_buffer) {
    pthread_mutex_lock(&circular_buffer->reader_mutex);
    while (circular_buffer->read_index == circular_buffer->write_index) {
        pthread_cond_wait(&circular_buffer->not_empty, &circular_buffer->reader_mutex);
    }
    int element = circular_buffer->buffer[circular_buffer->read_index];
    circular_buffer->read_index = (circular_buffer->read_index + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&circular_buffer->reader_mutex);
    pthread_cond_signal(&circular_buffer->not_full);
    return element;
}
