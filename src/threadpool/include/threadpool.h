#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include<vector>
class Task;
class Thread;
class ThreadPool
{
public:
	static ThreadPool* Get() {
		static ThreadPool pool;
		return &pool;
	}
	void Init(int thread_count);

	void Dispatch(Task* task);
private:
	unsigned int thread_count_ = 0;
	unsigned int last_thread_ = -1;
	std::vector<Thread*> threads_;
	ThreadPool(){
	}
};
#endif
