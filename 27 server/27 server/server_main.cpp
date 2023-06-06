#include "server_header.h"

#pragma comment(lib, "Ws2_32.lib")

int main() {
	//SOCKET

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);

	if (erStat <= 0) {
		std::cout << "Error in IP translation to special numeric format" << std::endl;
		return 1;
	}

	WSADATA wsData;

	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		std::cout << "Error WinSock version initializaion #";
		std::cout << WSAGetLastError();
		return 1;
	}
	else {
		std::cout << "WinSock initialization is OK" << std::endl;
	}

	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Server socket initialization is OK" << std::endl;
	}

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT_NUM);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		std::cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Binding socket to Server info is OK" << std::endl;
	}

	erStat = listen(ServSock, SOMAXCONN);

	if (erStat != 0) {
		std::cout << "Can't start to listen to. Error # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Listening..." << std::endl;
	}

	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << std::endl;
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Connection to a client established successfully" << std::endl;
		char clientIP[22];

		inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);

		std::cout << "Client connected with IP address " << clientIP << std::endl;
	}


	//MYSQL

	MYSQL mysql;
	mysql_init(&mysql);

	if (&mysql == NULL) {
		std::cout << "Error: can't create MySQL-descriptor" << std::endl;
	}

	if (!mysql_real_connect(&mysql, "localhost", "root", "feg123", "chatDB", 0, NULL, 0)) {
		std::cout << "Error: can't connect to database " << mysql_error(&mysql) << std::endl;
	}
	else {
		std::cout << "Success!" << std::endl;
	}

	mysql_set_character_set(&mysql, "utf8");
	std::cout << "connection characterset: " << mysql_character_set_name(&mysql) << std::endl;

	//fstream

	std::fstream logs("logs.txt");

	//main

	std::vector <char> Client_message(BUFF_SIZE);
	short smes = 0;
	std::string x, y;
	std::cout << "Waiting for the message" << std::endl;
	while (true) {
		recv(ClientConn, Client_message.data(), BUFF_SIZE, 0);

		if (Client_message[0] == '1') {
			Client_message.erase(Client_message.begin());
			std::cout << Client_message.data() << std::endl;
			x = Client_message.data();
			mysql_query(&mysql, "INSERT INTO user(id, name) values(default, x)");

			recv(ClientConn, Client_message.data(), BUFF_SIZE, 0);

			smes = send(ClientConn, Client_message.data(), BUFF_SIZE, 0);

			if (smes == SOCKET_ERROR) {
				std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			}

			std::cout << Client_message.data() << std::endl;
			y = Client_message.data();
			mysql_query(&mysql, "INSERT INTO user(id, name) values(default, y)");

			mysql_query(&mysql, "SELECT * FROM user");

			if (res = mysql_store_result(&mysql)) {
				while (row = mysql_fetch_row(res)) {
					for (int i = 0; i < mysql_num_fields(res); i++) {
						cout << row[i] << "  ";
					}
					std::cout << std::endl;
				}
			}
		}

		if (Client_message[0] == '3') {
			Client_message.erase(Client_message.begin());
			std::cout << Client_message.data() << std::endl;

			logs << (Client_message.data()) << std::endl;

			smes = send(ClientConn, Client_message.data(), BUFF_SIZE, 0);

			if (smes == SOCKET_ERROR) {
				std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			}
		}

		if (Client_message[0] == '4') {
			Client_message.erase(Client_message.begin());

		}
	}

	mysql_close(&mysql);
	logs.close();

	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();
	return 0;
}