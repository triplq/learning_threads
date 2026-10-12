#include <mutex>
#include <iostream>
#include <vector>
#include <thread>
#include <functional>

struct Account {
	int balance;
	std::mutex m;

	void send(int amount, Account& receiver) {
		std::scoped_lock lock(m, receiver.m);

		if (amount < 0 || amount > balance || this == &receiver)
			return;

		balance -= amount;
		receiver.balance += amount;
	}
};

void simmulate(int amount, Account& acc_send, Account& acc_receive) {
	for (size_t i = 0; i < 10000; i++) {
		acc_send.send(amount, acc_receive);
		acc_receive.send(amount, acc_send);
	}
}



int main() {
	Account a {1000};
	Account b {1000};

	std::thread th_a(simmulate, 100, std::ref(a), std::ref(b));
	std::thread th_b(simmulate, 100, std::ref(b), std::ref(a));

	th_a.join();
	th_b.join();

	std::cout << a.balance << ' ' << b.balance << '\n';

	return 0;
}