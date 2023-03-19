/**
   Reader index modified by reader ONLY, write index modified by writer ONLY.
   separate condition variable for isFull and isEmpty.
   In this version, there are two separate mutexes for the reader and writer threads,
   which allows them to run concurrently.
*/

#include <mutex>
#include <condition_variable>

#define BUFFER_SIZE 10

class CircularBuffer {
public:
    CircularBuffer() {
        write_index = 0;
        read_index = 0;
    }

// write enqueue   : cv variables efficient cycle wise
// semaphore has generic condition check versus CV can have a more specific check as per our need.
//mutex to protect shared state
// what problems are - what is sol to each problem

 void add_element(int element) {
        std::unique_lock<std::mutex> lock(writer_mutex);
        not_full.wait(lock, [this]() { return ((write_index + 1) % BUFFER_SIZE) != read_index; });
        buffer[write_index] = element;
        write_index = (write_index + 1) % BUFFER_SIZE;
        lock.unlock();
        not_empty.notify_one();
    }

    int remove_element() {
        std::unique_lock<std::mutex> lock(reader_mutex);
        not_empty.wait(lock, [this]() { return read_index != write_index; });
        int element = buffer[read_index];
        read_index = (read_index + 1) % BUFFER_SIZE;
        lock.unlock();
        not_full.notify_one();
        return element;
    }

private:
    int buffer[BUFFER_SIZE + 1]; /* We need 1 extra space to be able to determine queue is not empty in absence of a shared length variable */
    std::mutex reader_mutex;
    std::mutex writer_mutex;
    std::condition_variable not_full;
    std::condition_variable not_empty;
    size_t write_index;
    size_t read_index;
};
