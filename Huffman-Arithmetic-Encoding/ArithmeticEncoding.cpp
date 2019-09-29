#include "ArithmeticEncoding.h"

std::tuple<unsigned long long, unsigned long long, unsigned long long> 
GetProbability(const unsigned char & c, std::vector<unsigned long long>& cumulative_frequency)
{
  int index = (unsigned int)c;
  auto result = std::make_tuple(cumulative_frequency[c], cumulative_frequency[c + 1], cumulative_frequency[257]);
  return result;
}

void Update(const unsigned char& c, std::vector<unsigned long long>& cumulative_frequency)
{
  int index = (unsigned int)c;
  for (int i = c + 1; i < 258; i++)
    cumulative_frequency[i]++;
}

void OutputBitPlusPending(bool bit, int& pendingBits)
{
  std::cout << bit;
  while (pendingBits)
  {
    --pendingBits;
    std::cout << !bit;
  }
}

unsigned char GetSymbol(double long d)
{
  unsigned char c = 0;
  c = c + (unsigned int)(d * 255);
  return c;
}

void ArithmeticEncoding(std::string fn)
{
  unsigned char high = 254;
  unsigned char low = 0;
  int pendingBits = 0;

  std::string tmpText("WXYTAZ");

  unsigned long long N = tmpText.size();

  std::map<unsigned char, unsigned long long> cumulative;

  for (auto& c : tmpText)
    ++cumulative[c];

  for (auto cumu = ++cumulative.begin(); cumu != cumulative.end(); ++cumu)
  {
    auto prev = cumu;
    --prev;
    cumu->second += prev->second;
  }

  cumulative[0] = 0;

  for (auto& c : cumulative)
    std::cout << c.first << " " << c.second << std::endl;

  for (auto& c : tmpText)
  {
    unsigned char range = high - low + 0x1;
    auto cumu = cumulative.find(c);
    auto prev = cumu;
    --prev;
    high = low + ((range * cumu->second) / N) - 1;
    low  = low + (range * prev->second) / N;
    while (true) 
    {
      if (high < 128)
      {
        OutputBitPlusPending(0, pendingBits);
        // std::cout << (int)high << " " << (int)low << std::endl;
        low <<= 1;
        high = (high << 1) + 1;
      }
      else if (low >= 128)
      {
        OutputBitPlusPending(1, pendingBits);
        // std::cout << (int)high << " " << (int)low << std::endl;
        low <<= 1;
        high = (high << 1) + 1;
      }
      else if (low >= 64 && high < 192)
      {
        ++pendingBits;
        high = ((high - 64) << 1) + 1;
        low = (low - 64) << 1;
      }
      else break;
    }
  }
  
  // std::cout << "Pending Bits : " << pendingBits << std::endl;
  if (pendingBits)
  {
    OutputBitPlusPending(0, pendingBits);
    std::cout << std::endl << "Pending Bits : " << pendingBits;
  }
  std::cout << std::endl;
}

void ArithmeticDecoding(std::string fn)
{
  std::string encodedData("011011101101100");

  std::string tmpText("WXYTAZ");

  unsigned long long N = tmpText.size();

  std::map<unsigned char, unsigned long long> cumulative;

  for (auto& c : tmpText)
    ++cumulative[c];

  for (auto cumu = ++cumulative.begin(); cumu != cumulative.end(); ++cumu)
  {
    auto prev = cumu;
    --prev;
    cumu->second += prev->second;
  }

  cumulative[0] = 0;

  for (auto& c : cumulative)
    std::cout << c.first << " " << c.second << std::endl;

  unsigned int counter = 0;

  unsigned char high = 254;
  unsigned char low = 0;


 
  bitstring val;
  for (; counter < 8; ++counter)
  {
    if (counter < encodedData.size())
    {
      if (encodedData[counter] == '1') val.push_back(true);
      else val.push_back(false);
    }
    else val.push_back(false);
  }
  std::cout << val.size() << std::endl;
  val.Generate();
  std::cout << val.String() << std::endl;
  
  // decoding here have to think of how to send header
  while (true)
  {
    unsigned char range = high - low + 0x1;
    auto n = ++cumulative.begin();
    unsigned char value = (unsigned char)val.Value();
    // according to slide is suppose to mean eof
    
    while (value > low + ((range * n->second) / N) - 1)
      ++n;
    std::cout << n->first;
    auto prev = n;
    --prev;
    // std::cout << val.String() << std::endl;
    high = low + ((range * n->second) / N) - 1;
    low = low + ((range * prev->second) / N);

    // scaling here
    while (true)
    {
      if (low >= 128 || high < 128)
      {
        low = low << 1;
        high = (high << 1) + 1;
        val.LeftShift();
        if(counter < encodedData.size())
          val.ReadBit(encodedData[counter++]);
        // std::cout << "E2 " << val.String() << std::endl;
        // std::cout << "low "  << (int)low << std::endl;
        // std::cout << "high " << (int)high << std::endl;
      }
      else if (low >= 64 && high < 192)
      {
        low = (low - 64) << 1;
        high = ((high - 64) << 1) + 1;
        unsigned char tmp = (unsigned char)val.Value();
        tmp = (tmp - 64) << 1;
        if (counter < encodedData.size() && encodedData[counter++]) tmp = tmp | 0x1;
        val.ChangeValue(tmp);
        // std::cout << "E3 " << val.String() << std::endl;
        // std::cout << "low " << (int)low << std::endl;
        // std::cout << "high " << (int)high << std::endl;
      }
      else break;
    }
    if (value == low) break;
    
  }
  std::cout << std::endl;
}
