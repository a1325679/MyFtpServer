#include "threadpool.h"
#include"threads.h"
#include<thread>
#include<iostream>
using namespace std;
void ThreadPool::Init(int thread_count) {
	this->thread_count_ = thread_count;
	this->last_thread_ = -1;
	for (int i = 0; i < thread_count; i++) {
		Thread* t = new Thread();
		t->id_ = i + 1;
		t->Start();
		threads_.push_back(t);
		this_thread::sleep_for(10ms);
	}
}

void ThreadPool::Dispatch(Task* task) {
	//轮询
  if (!task)
    return;
  int tid = (last_thread_ + 1) % thread_count_;
	last_thread_ = tid;
	Thread* t = threads_[tid];
	t->AddTask(task);

	//激活线程
	t->Activate();
}