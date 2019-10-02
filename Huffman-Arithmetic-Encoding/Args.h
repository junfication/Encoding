#pragma once
#include <string>

enum class MODE : char
{
  NO_MODE = -1,
  LOCAL_CODING = 0,
  SENDER = 1,
  RECIEVER = 2
};

enum class TYPE : char
{
  NO_TYPE = -1,
  HUFFMAN_ENCODE = 0,
  HUFFMAN_DECODE = 1,
  ARITHMETIC_ENCODE = 2,
  ARITHMETIC_DECODE = 3,
  HUFFMAN = 4,
  ARITHMETIC = 5
};


struct ArgumentsParser
{
  std::string input;
  std::string output;
  MODE mode = MODE::NO_MODE;
  TYPE operation = TYPE::NO_TYPE;

  bool ParseArguments(char** args);
};
