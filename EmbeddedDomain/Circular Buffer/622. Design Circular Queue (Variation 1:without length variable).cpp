/**
622. Design Circular Queue 
https://leetcode.com/problems/design-circular-queue/description/
*/


class MyCircularQueue {
private:
        vector<int> q;//[1000];
        //int front, rear, size;
        int front;
        int rear;
        int size;

public:
    MyCircularQueue(int k): q(k) {
        size = k;
        front = -1;
        rear = -1;
    }
    
    bool enQueue(int value) {
        if(!isFull()){
            //adjust the front only if Q is empty
            if(isEmpty()){
                front = front+1;
            }
            rear = (rear+1) % (size);
            q[rear] = value;
            return true;
        }
        else{
            return false;
        }
    }
    
    bool deQueue() {
        if(!isEmpty()){
            //if only single element in the Q, then we need to adjust the rear too
            if(front == rear){
                rear = -1;
                front = -1;
            }
            else{
                front = (front +1)% size;
            }
            return true;
        }
        else{
            return false;
        }
    }
    
    int Front() {
        return isEmpty() ? -1 : q[front];
    }
    
    int Rear() {
        return isEmpty() ? -1 : q[rear];
    }
    
    bool isEmpty() {
        return (front == -1)? true : false;
    }
    
    bool isFull() {
        if(front == (rear + 1) % size){
            return true;
        }
        else{
            return false;
        }
    }
};

/**
 * Your MyCircularQueue object will be instantiated and called as such:
 * MyCircularQueue* obj = new MyCircularQueue(k);
 * bool param_1 = obj->enQueue(value);
 * bool param_2 = obj->deQueue();
 * int param_3 = obj->Front();
 * int param_4 = obj->Rear();
 * bool param_5 = obj->isEmpty();
 * bool param_6 = obj->isFull();
 */
