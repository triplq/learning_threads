#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

std::mutex m;
std::vector<int> tasks;
std::condition_variable cv;
bool stopped = false;

void worker_1() {
	int task;

	while (true) {
		std::unique_lock<std::mutex> lock(m);

		cv.wait(lock, [&]{ 
			return !tasks.empty() || stopped; });

		if (tasks.empty() && stopped)
			return;

		task = tasks.back();
		tasks.pop_back();

		std::cout << "Работник 1 получил задачу: " << task << '\n';
	}
}

void worker_2() {
	int task;

	while (true) {
		std::unique_lock<std::mutex> lock(m);

		cv.wait(lock, [&]{ 
			return !tasks.empty() || stopped; });

		if (tasks.empty() && stopped)
			return;

		task = tasks.back();
		tasks.pop_back();

		std::cout << "Работник 2 получил задачу: " << task << '\n';
	}
}

void line() {
	for (size_t i = 0; i < 1000; i++) {
		{
			std::lock_guard<std::mutex> lk(m);
			tasks.push_back(i);
		}
		cv.notify_one();
	}

	{
		std::lock_guard<std::mutex> lk(m);
		stopped = true;
	}

	cv.notify_all();
}

int main() {
	std::thread th_worker_1(worker_1);
	std::thread th_worker_2(worker_2);

	std::thread th_line(line);

	th_worker_1.join();
	th_worker_2.join();
	th_line.join();

	std::cout << "Раздача закончена\n";

	return 0;
}