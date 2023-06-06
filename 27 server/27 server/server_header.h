#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include "mysql.h"

const char IP_SERV[] = "127.0.0.1";
const int PORT_NUM = 7777;
const short BUFF_SIZE = 256;
int erStat;
MYSQL_RES* res;
MYSQL_ROW row;
