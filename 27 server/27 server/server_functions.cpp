#include "server_header.h"

void Locker::add_log(std::string log) {
	logs.push_back(log);
}

void Locker::show_log() {
	int x;
	std::cout << "Enter number" << std::endl;
	std::cin >> x;
	std::cout << logs[x] << std::endl;
}