#pragma once
#include <string>
#include <iostream>
#include <limits>
#include <tuple>
#include <vector>
#include <map>
#include "bitstring.h"

#define probability 1.0/255.0

std::tuple<unsigned long long, unsigned long long, unsigned long long> 
GetProbability(const unsigned char& c, std::vector<unsigned long long>& cumulative_frequency);

void Update(const unsigned char& c, std::vector<unsigned long long>& cumulative_frequency);

void OutputBitPlusPending(bool bit, int& pendingBits);

unsigned char GetSymbol(double long d);
void ArithmeticEncoding(std::string fn);
void ArithmeticDecoding(std::string fn);