#pragma once
#include <vector>
#include <string>
#include <iostream>

struct bitstring : std::vector<bool>
{
private:
  std::string string;
  unsigned int value = 0;
  void GenerateString();
  void GenerateValue();
public:
  bool eof = false;
  
  std::string Output();

  void ChangeValue(unsigned int val);

  unsigned int Value() const
  {
    return value;
  }

  std::string String() const
  {
    return string;
  }

  void Generate();

  bitstring& operator+=(const bitstring& bs);
  bool operator==(const bitstring& bs);
  bool operator<(const bitstring& bs);

  void LeftShift();
  void ReadBit(char c);
};