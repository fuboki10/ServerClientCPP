#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

#ifndef ThreadSafeQueue_H
#define ThreadSafeQueue_H

class ThreadSafeQueue
{
private:
  queue<int> m_queue;
  mutex m_mutex;
  condition_variable cv;

public:
  ThreadSafeQueue() {}
  void push(int data);
  int pop();
};

#endif // ThreadSafeQueue_H