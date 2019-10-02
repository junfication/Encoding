#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "bitstring.h"

#define USE_ENG_PROBS 1

void OutputBitPlusPending(bool bit, int& pendingBits, bitstring& bs, bool DEBUG);

void ArithmeticEncoding(const std::string& fn, const std::string& outFile);
std::string ArithmeticEncodingHelper(std::string inputData);
void ArithmeticDecoding(const std::string& fn, const std::string& outFile);
std::string ArithmeticDecodingHelper(std::string inputData);