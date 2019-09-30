#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "bitstring.h"

#define USE_ENG_PROBS 1

void OutputBitPlusPending(bool bit, int& pendingBits, bitstring& bs, bool DEBUG);

void ArithmeticEncoding(std::string fn);
void ArithmeticDecoding(std::string fn);