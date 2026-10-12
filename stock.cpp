#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>
#include <chrono>

class Stock {
private:
	mutable std::mutex m_;
	int items_ = 10;
	
public:
	bool try_take() {
		std::lock_guard<std::mutex> lk(m_);
		if (items_ > 0) {
			items_--;
			return true;
		}
		return false;
	}

	int count() const {
		std::lock_guard<std::mutex> lk(m_);
		return items_;
	}

	
};

void buyer(Stock& s) {
	if (s.try_take()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

int main() {
	std::vector<std::thread> ths;
	ths.reserve(20);

	Stock s;

	for (size_t i = 0; i < 20; i++) {
		ths.emplace_back(buyer, std::ref(s));
	}

	for (auto& th : ths) {
		th.join();
	}

	std::cout << s.count() << '\n';
}