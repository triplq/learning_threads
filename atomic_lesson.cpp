#include <iostream>
#include <thread>
#include <vector>
// #include <mutex>
#include <atomic>

// std::mutex m;
std::atomic<int> balance = 100;



void add() {
	// std::lock_guard<std::mutex> lk(m);
	balance++;
}

void sub() {
	// std::lock_guard<std::mutex> lk(m);
	balance--;
}

int main() {
	std::vector<std::thread> ths;
	ths.reserve(4);

	for (int i = 0; i < 2; i++) {
		ths.emplace_back(add);
	}

	for (int i = 0; i < 2; i++) {
		ths.emplace_back(sub);
	}

	for (int i = 0; i < 4; i++) {
		ths[i].join();
	}

	std::cout << balance << '\n';

	return 0;
}