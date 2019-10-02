#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// includes
#include <windows.h>
#include <winsock2.h> // winsock
#include <ws2tcpip.h> // winsock functions
#include <iostream> // std::cin, std::getline
#include <string> // std::string
#include <algorithm> // std::copy
#include "Args.h"
#include "HuffmanEncoding.h"
#include "ArithmeticEncoding.h"


#pragma comment(lib, "Ws2_32.lib")

void Sender(const ArgumentsParser& ap);
void Reciever(const ArgumentsParser& ap);