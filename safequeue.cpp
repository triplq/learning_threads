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
	SafeQueue() : queue(std::make_unique<T[]>(1)), size(0), capacity(1) { }

	void push(T t) {
		{
			std::lock_guard<std::mutex> lk(m);
			if (size == capacity) 
				increase_capacity();

			queue[size] = t;
			size++;
		}
		cv.notify_one();
	}

	bool wait_and_pop(T& out) {
		T task;
		while(true) {
			std::unique_lock<std::mutex> lock(m);

			cv.wait(lock, [this]{
				return size != 0 || stopped;
			});

			if (size == 0 && stopped) {
				return false;
			}

			task = queue[size];
			size--;
		}

		out = task;
	}

	void close() {
		std::lock_guard<std::mutex> lk(m);
		stopped = true;
		cv.notify_all();
	}
};

int main() {
	SafeQueue<int> sq;
	int out;
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
		consumers.emplace_back(&SafeQueue<int>::wait_and_pop, &sq, std::ref(out));
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