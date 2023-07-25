#include "ThreadSafeQueue.h"

void ThreadSafeQueue::push(int data)
{
  unique_lock<mutex> lock(m_mutex);
  m_queue.push(data);
  cv.notify_one();
}

int ThreadSafeQueue::pop()
{
  unique_lock<mutex> lock(m_mutex);
  cv.wait(lock,
          [this]()
          { return !m_queue.empty(); });
  int data = m_queue.front();
  m_queue.pop();
  return data;
}
