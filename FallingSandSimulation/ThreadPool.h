#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool
{
public:
	size_t numThreads;

	ThreadPool(size_t numThreads);
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	const ThreadPool& operator=(const ThreadPool&) = delete;
	const ThreadPool& operator=(ThreadPool&&) = delete;

	template<typename F, class... Args>
	std::future<std::invoke_result_t<F, Args...>> enqueue(F&& f, Args&&... args)
	{
		using returnType = std::invoke_result_t<F, Args...>;

		auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<returnType> result = task->get_future();

		{
			std::unique_lock<std::mutex> lock(queueMutex);

			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks.emplace([task]() { (*task)(); });
		}

		condition.notify_one();
		return result;
	}

private:
	bool stop;

	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;

	std::mutex queueMutex;
	std::condition_variable condition;
};