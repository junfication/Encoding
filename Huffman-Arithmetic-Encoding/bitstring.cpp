#include "bitstring.h"

void bitstring::GenerateString()
{
  string.clear();
  for (auto start = begin(); start != end(); ++start)
  {
    if (*start) string.push_back('1');
    else string.push_back('0');
  }
}

void bitstring::GenerateValue()
{
  unsigned int bit = 0x1;
  auto start = --end();
  for (; start != begin(); --start)
  {
    if (*start) value = value | bit;
    bit = bit << 1;
  }
  if (*start) value = value | bit;
}

std::string bitstring::Output()
{
  std::string data;
  std::vector<bool> tmp = *this;
  unsigned missing = tmp.size() % 8;
  missing = missing == 0 ? 0 : 8 - missing;
  for (unsigned i = 0; i < missing; ++i)
    tmp.push_back(false);
  for (unsigned i = 0; i < tmp.size(); i += 8)
  {
    unsigned index = i + 7;
    unsigned char bit = 0x1;
    unsigned char val = 0;
    for (; index != i; --index)
    {
      if (tmp[index]) val = val | bit;
      bit = bit << 1;
    }
    if (tmp[index]) val = val | bit;
    data.push_back(val);
  }
  return data;
}

void bitstring::ChangeValue(unsigned int val)
{
  value = val;
  unsigned int bit = 0x1;
  auto start = end();
  for (--start; start != begin(); --start)
  {
    unsigned int result = bit & value;
    if (result) *start = true;
    else *start = false;
    bit = bit << 1;
  }
  unsigned int result = bit & value;
  if (result) *start = true;
  else *start = false;
  GenerateString();
}

void bitstring::Generate()
{
  GenerateString();
  GenerateValue();
}

bitstring& bitstring::operator+=(const bitstring& bs)
{
  for (auto start = bs.begin(); start != bs.end(); ++start)
    push_back(*start);
  return *this;
}

bool bitstring::operator==(const bitstring& bs)
{
  if (size() != bs.size()) return false;
  size_t sz = size();
  for(size_t i = 0; i < sz; ++i)
  {
    if (bs[i] != operator[](i)) return false;
  }
  return true;
}

bool bitstring::operator<(const bitstring& bs)
{
  return size() < bs.size();
}
