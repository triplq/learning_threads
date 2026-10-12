#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class SafeCounter {
private:
	int counter_ = 0;
	mutable std::mutex m;

public:
	SafeCounter() {};

	void inc() {
		std::lock_guard<std::mutex> lk(m);
		counter_++;
	}
	std::string get() const {
		std::lock_guard<std::mutex> lk(m);
		return std::to_string(counter_);
	}
};

int main() {
	std::vector<std::thread> ths;
	ths.reserve(8);

	SafeCounter counter;

	for (size_t i = 0; i < 8; i++) {
		ths.emplace_back([&](){
			for (size_t i = 0; i < 10000; i++) {
				counter.inc();
			}
		});
	}

	for (auto& th : ths) {
		th.join();
	}

	std::cout << counter.get() << '\n';

	return 0;
}