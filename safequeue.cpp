#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include <functional>

template <class T>
class SafeQueue {
private:
	std::mutex m;
	std::unique_ptr<T[]> queue;
	size_t size;
	size_t begin;
	size_t capacity;
	bool stopped = false;
	std::condition_variable cv;

	void increase_capacity() {
		std::unique_ptr<T[]> queue_n = std::make_unique<T[]>(capacity * 2);
		for(size_t i = 0; i < size; i++) {
			queue_n[i] = std::move(queue[i]);
		}

		queue = std::move(queue_n);
		capacity *= 2;
	}

public:
	SafeQueue() : queue(std::make_unique<T[]>(1)), size(0), begin(0), capacity(1) { }

	void push(T t) {
		if (!stopped) {
			{
				std::lock_guard<std::mutex> lk(m);
				if (size == capacity) 
					increase_capacity();

				queue[size] = std::move(t);
				size++;
			}
			cv.notify_one();
		}
	}

	bool wait_and_pop(T& out) {
		T task;
		{
			std::unique_lock<std::mutex> lock(m);

			cv.wait(lock, [this]{
				return begin != size || stopped;
			});

			if (size == begin && stopped) {
				return false;
			}

			if (begin != size) {
				task = queue[begin];
				begin++;
				out = task;
			}
		}

		return true;	
	}

	void close() {
		std::lock_guard<std::mutex> lk(m);
		stopped = true;
		cv.notify_all();
	}
};

int main() {
	SafeQueue<int> sq;
	std::vector<std::thread> producers;
	producers.reserve(2);

	std::vector<std::thread> consumers;
	consumers.reserve(3);

	for (size_t i = 0; i < 2; i++) {
		producers.emplace_back([&]{
			for (size_t i = 0; i < 1000; i++) {
				sq.push(i);
			}
		});
	}

	for (size_t i = 0; i < 3; i++) {
		consumers.emplace_back([&]{
			int out;
			while (sq.wait_and_pop(out)) {
				// std::cout << out << '\n';
			}
		});
	}

	for (auto& p : producers) {
		p.join();
	}
	sq.close();
	for (auto& c : consumers) {
		c.join();
	}

	return 0;
}