#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <chrono>
#include <queue>
#include "bitstring.h"

struct Node
{
  unsigned long long weight = 0;
  char symbol = NULL;
  Node* left = nullptr;
  Node* right = nullptr;
  bool eof = false;
  void clear();
  std::vector<std::pair<char, bitstring>> GenerateTable();
private:
  void DeleteHelper(Node* n);
  void TableHelper(Node* n, bitstring bs, std::vector<std::pair<char, bitstring>>& t);
};

void HuffmanEncoding(const std::string& fn, const std::string& outFile);
std::string HuffmanEncoderHelper(std::string inputData);
void HuffmanDecoding(const std::string& fn, const std::string& outFile);
std::string HuffmanDecoderHelper(std::string inputData);