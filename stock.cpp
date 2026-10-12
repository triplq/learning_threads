#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>

class Stock {
private:
	mutable std::mutex m_;
	int items_ = 10;
public:
	int count() const {
		std::lock_guard<std::mutex> lk(m_);
		return items_;
	}
	void take() {
		std::lock_guard<std::mutex> lk(m_);
		items_--;
	}
};

void buyer(Stock& s) {
	if (s.count() > 0) {
		s.take();
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