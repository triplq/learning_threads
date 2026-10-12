#include <mutex>
#include <iostream>
#include <vector>
#include <thread>

struct Account {
	int balance;
	std::mutex m;

	void send(int amount, Account& receiver) {
		if (amount < 0 || amount > balance)
			return;

		balance -= amount;
		receiver.balance += amount;
	}
}



int main() {
	std::thread th_a;
	std::thread th_b;

	for (size_t i = 0; i < 10000; i++) {

	}


	return 0;
}