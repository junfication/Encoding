#include "ArithmeticEncoding.h"

void OutputBitPlusPending(bool bit, int& pendingBits, bitstring& bs, bool DEBUG = 0)
{
  if(DEBUG) std::cout << bit;
  bs.push_back(bit);
  while (pendingBits)
  {
    --pendingBits;
    if(DEBUG) std::cout << !bit;
    bs.push_back(!bit);
  }
}

void ArithmeticEncoding(const std::string& fn, const std::string& outFile)
{
  std::ifstream input(fn, std::ios::binary);
  if (!input.is_open())
  {
    std::cout << "No file is opened" << std::endl;
    return;
  }
  std::string inputData(std::istreambuf_iterator<char>(input), {});

  std::string output = ArithmeticEncodingHelper(inputData);

  // output to file here
  std::ofstream file(outFile, std::ios::binary);
  file << output;
  file.close();
  input.close();
}

std::string ArithmeticEncodingHelper(std::string inputData)
{
#if USE_ENG_PROBS
  //this table is based on the frequency of letters in english text
  //for other symbols, they are assigned a probability of 1
  const unsigned short probabilitiesHigh[257] =
  {
    2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 278, 305, 354, 432, 651, 693, 729, 836, 969, 970, 982, 1054, 1101, 1227, 1367, 1400, 1402, 1511, 1625, 1790, 1842, 1862, 1900, 1903, 1941, 1942, 1944, 1946, 1948, 1950, 1952, 1954, 2103, 2131, 2181, 2260, 2480, 2523, 2560, 2668, 2802, 2804, 2817, 2890, 2938, 3065, 3206, 3240, 3243, 3353, 3468, 3634, 3687, 3708, 3747, 3751, 3790, 3792, 3794, 3796, 3798, 3800, 3802, 3804, 3806, 3808, 3810, 3812, 3814, 3816, 3818, 3820, 3822, 3824, 3826, 3828, 3830, 3832, 3834, 3836, 3838, 3840, 3842, 3844, 3846, 3848, 3850, 3852, 3854, 3856, 3858, 3860, 3862, 3864, 3866, 3868, 3870, 3872, 3874, 3876, 3878, 3880, 3882, 3884, 3886, 3888, 3890, 3892, 3894, 3896, 3898, 3900, 3902, 3904, 3906, 3908, 3910, 3912, 3914, 3916, 3918, 3920, 3922, 3924, 3926, 3928, 3930, 3932, 3934, 3936, 3938, 3940, 3942, 3944, 3946, 3948, 3950, 3952, 3954, 3956, 3958, 3960, 3962, 3964, 3966, 3968, 3970, 3972, 3974, 3976, 3978, 3980, 3982, 3984, 3986, 3988, 3990, 3992, 3994, 3996, 3998, 4000, 4002, 4004, 4006, 4008, 4010, 4012, 4014, 4016, 4018, 4020, 4022, 4024, 4026, 4028, 4030, 4032, 4034, 4036, 4038, 4040, 4042, 4044, 4046, 4048, 4050, 4052, 4054, 4056, 4058, 4059
};

  const unsigned short probabilitiesLow[257] =
  {
    0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 278, 305, 354, 432, 651, 693, 729, 836, 969, 970, 982, 1054, 1101, 1227, 1367, 1400, 1402, 1511, 1625, 1790, 1842, 1862, 1900, 1903, 1941, 1942, 1944, 1946, 1948, 1950, 1952, 1954, 2103, 2131, 2181, 2260, 2480, 2523, 2560, 2668, 2802, 2804, 2817, 2890, 2938, 3065, 3206, 3240, 3243, 3353, 3468, 3634, 3687, 3708, 3747, 3751, 3790, 3792, 3794, 3796, 3798, 3800, 3802, 3804, 3806, 3808, 3810, 3812, 3814, 3816, 3818, 3820, 3822, 3824, 3826, 3828, 3830, 3832, 3834, 3836, 3838, 3840, 3842, 3844, 3846, 3848, 3850, 3852, 3854, 3856, 3858, 3860, 3862, 3864, 3866, 3868, 3870, 3872, 3874, 3876, 3878, 3880, 3882, 3884, 3886, 3888, 3890, 3892, 3894, 3896, 3898, 3900, 3902, 3904, 3906, 3908, 3910, 3912, 3914, 3916, 3918, 3920, 3922, 3924, 3926, 3928, 3930, 3932, 3934, 3936, 3938, 3940, 3942, 3944, 3946, 3948, 3950, 3952, 3954, 3956, 3958, 3960, 3962, 3964, 3966, 3968, 3970, 3972, 3974, 3976, 3978, 3980, 3982, 3984, 3986, 3988, 3990, 3992, 3994, 3996, 3998, 4000, 4002, 4004, 4006, 4008, 4010, 4012, 4014, 4016, 4018, 4020, 4022, 4024, 4026, 4028, 4030, 4032, 4034, 4036, 4038, 4040, 4042, 4044, 4046, 4048, 4050, 4052, 4054, 4056, 4058
  };
#else
  //this is a linear normal distribution, with max probability at the center value, i.e 127,
  //and minimum probability at max and minimum char value
  const unsigned short int probabilitiesHigh[257] =
  {
    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210, 231, 253, 276, 300, 325, 351, 378, 406, 435, 465, 496, 528, 561, 595, 630, 666, 703, 741, 780, 820, 861, 903, 946, 990, 1035, 1081, 1128, 1176, 1225, 1275, 1326, 1378, 1431, 1485, 1540, 1596, 1653, 1711, 1770, 1830, 1891, 1953, 2016, 2080, 2145, 2211, 2278, 2346, 2415, 2485, 2556, 2628, 2701, 2775, 2850, 2926, 3003, 3081, 3160, 3240, 3321, 3403, 3486, 3570, 3655, 3741, 3828, 3916, 4005, 4095, 4186, 4278, 4371, 4465, 4560, 4656, 4753, 4851, 4950, 5050, 5151, 5253, 5356, 5460, 5565, 5671, 5778, 5886, 5995, 6105, 6216, 6328, 6441, 6555, 6670, 6786, 6903, 7021, 7140, 7260, 7381, 7503, 7626, 7750, 7875, 8001, 8128, 8256, 8383, 8509, 8634, 8758, 8881, 9003, 9124, 9244, 9363, 9481, 9598, 9714, 9829, 9943, 10056, 10168, 10279, 10389, 10498, 10606, 10713, 10819, 10924, 11028, 11131, 11233, 11334, 11434, 11533, 11631, 11728, 11824, 11919, 12013, 12106, 12198, 12289, 12379, 12468, 12556, 12643, 12729, 12814, 12898, 12981, 13063, 13144, 13224, 13303, 13381, 13458, 13534, 13609, 13683, 13756, 13828, 13899, 13969, 14038, 14106, 14173, 14239, 14304, 14368, 14431, 14493, 14554, 14614, 14673, 14731, 14788, 14844, 14899, 14953, 15006, 15058, 15109, 15159, 15208, 15256, 15303, 15349, 15394, 15438, 15481, 15523, 15564, 15604, 15643, 15681, 15718, 15754, 15789, 15823, 15856, 15888, 15919, 15949, 15978, 16006, 16033, 16059, 16084, 16108, 16131, 16153, 16174, 16194, 16213, 16231, 16248, 16264, 16279, 16293, 16306, 16318, 16329, 16339, 16348, 16356, 16363, 16369, 16374, 16378, 16381, 16383, 16384, 16385, 16386
  };

  const unsigned short int probabilitiesLow[257] =
  {
    0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210, 231, 253, 276, 300, 325, 351, 378, 406, 435, 465, 496, 528, 561, 595, 630, 666, 703, 741, 780, 820, 861, 903, 946, 990, 1035, 1081, 1128, 1176, 1225, 1275, 1326, 1378, 1431, 1485, 1540, 1596, 1653, 1711, 1770, 1830, 1891, 1953, 2016, 2080, 2145, 2211, 2278, 2346, 2415, 2485, 2556, 2628, 2701, 2775, 2850, 2926, 3003, 3081, 3160, 3240, 3321, 3403, 3486, 3570, 3655, 3741, 3828, 3916, 4005, 4095, 4186, 4278, 4371, 4465, 4560, 4656, 4753, 4851, 4950, 5050, 5151, 5253, 5356, 5460, 5565, 5671, 5778, 5886, 5995, 6105, 6216, 6328, 6441, 6555, 6670, 6786, 6903, 7021, 7140, 7260, 7381, 7503, 7626, 7750, 7875, 8001, 8128, 8256, 8383, 8509, 8634, 8758, 8881, 9003, 9124, 9244, 9363, 9481, 9598, 9714, 9829, 9943, 10056, 10168, 10279, 10389, 10498, 10606, 10713, 10819, 10924, 11028, 11131, 11233, 11334, 11434, 11533, 11631, 11728, 11824, 11919, 12013, 12106, 12198, 12289, 12379, 12468, 12556, 12643, 12729, 12814, 12898, 12981, 13063, 13144, 13224, 13303, 13381, 13458, 13534, 13609, 13683, 13756, 13828, 13899, 13969, 14038, 14106, 14173, 14239, 14304, 14368, 14431, 14493, 14554, 14614, 14673, 14731, 14788, 14844, 14899, 14953, 15006, 15058, 15109, 15159, 15208, 15256, 15303, 15349, 15394, 15438, 15481, 15523, 15564, 15604, 15643, 15681, 15718, 15754, 15789, 15823, 15856, 15888, 15919, 15949, 15978, 16006, 16033, 16059, 16084, 16108, 16131, 16153, 16174, 16194, 16213, 16231, 16248, 16264, 16279, 16293, 16306, 16318, 16329, 16339, 16348, 16356, 16363, 16369, 16374, 16378, 16381, 16383, 16384, 16385
  };
#endif

  auto then = std::chrono::system_clock::now();

  unsigned short int low = 0;
  unsigned short int high = 0xffff;
  int pendingBits = 0;
  bitstring bs;

  for (unsigned char c : inputData)
  {
    unsigned short int lowCount;
    unsigned short int highCount;
    unsigned short int scale = probabilitiesHigh[256];

    lowCount = probabilitiesLow[c];
    highCount = probabilitiesHigh[c];

    long range = (long)(high - low) + 1;
    high = low + (unsigned short int)((range * highCount) / scale - 1);
    low = low + (unsigned short int)((range * lowCount) / scale);

    while (true)
    {
      if (high < 0x8000)
      {
        OutputBitPlusPending(0, pendingBits, bs);
      }
      else if (low >= 0x8000)
      {
        OutputBitPlusPending(1, pendingBits, bs);
      }
      else if ((low >= 0x4000) && (high < 0xC000))
      {
        ++pendingBits;
        low &= 0x3fff;
        high |= 0x4000;
      }
      else
        break;
      low <<= 1;
      high <<= 1;
      high |= 1;
    }
  }

  // encode EOF
  unsigned short int lowCount;
  unsigned short int highCount;
  unsigned short int scale = probabilitiesHigh[256]; //non adaptive model

  lowCount = probabilitiesLow[256];
  highCount = probabilitiesHigh[256];

  long range = (long)(high - low) + 1;
  high = low + (unsigned short int)((range * highCount) / scale - 1);
  low = low + (unsigned short int)((range * lowCount) / scale);

  while (true)
  {
    if (high < 0x8000)
    {
      OutputBitPlusPending(0, pendingBits, bs);
    }
    else if (low >= 0x8000)
    {
      OutputBitPlusPending(1, pendingBits, bs);
    }
    else if ((low >= 0x4000) && (high < 0xC000))
    {
      ++pendingBits;
      low &= 0x3fff;
      high |= 0x4000;
    }
    else
      break;
    low <<= 1;
    high <<= 1;
    high |= 1;
  }
  // end of encoding EOF

  if (++pendingBits)
  {
    if (low & 0x4000)
      OutputBitPlusPending(1, pendingBits, bs);
    else
      OutputBitPlusPending(0, pendingBits, bs);
  }
  bs.Generate();
  auto output = bs.Output();

  if (output.size() != 0 && (output.size() % 2 == 1)) output.push_back(0);

  // generate header

  std::string header;
  header.resize(514);
  int headerCounter = 0;

  for (int i = 0; i < 257; ++i)
  {
    unsigned short val = probabilitiesHigh[i];
    char* valPtr = (char*)((void*)&val);
    header[headerCounter++] = *valPtr;
    header[headerCounter++] = *(valPtr + 1);
  }

  // end of generate header

  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> duration = now - then;
  std::cout << "Encoding Time Taken : " << duration.count() * 1000.0 << " milliseconds" << std::endl;

  long double uncompressedSz = inputData.size();
  long double compressedSz = output.size();

  std::cout << "Compression rate : " << uncompressedSz / compressedSz << std::endl;

  output = header + output;
  return output;
}

void ArithmeticDecoding(const std::string& fn, const std::string& outFile)
{
  std::ifstream input(fn, std::ios::binary);
  if (!input.is_open())
  {
    std::cout << "No file is opened" << std::endl;
    return;
  }
  std::string inputData(std::istreambuf_iterator<char>(input), {});

  std::string output = ArithmeticDecodingHelper(inputData);

  // output to file
  std::ofstream file(outFile, std::ios::binary);
  file << output;
  file.close();
  input.close();
}

std::string ArithmeticDecodingHelper(std::string inputData)
{
  auto then = std::chrono::system_clock::now();

  unsigned short probabilitiesHighD[257];
  unsigned short probabilitiesLowD[257];

  memset(&probabilitiesHighD, 0, 257 * sizeof(short));
  memset(&probabilitiesLowD, 0, 257 * sizeof(short));

  unsigned short int low = 0;
  unsigned short int high = 0xffff;
  int count;
  size_t counter = 0;
  unsigned short int codeVal = 0;
  std::string output;

  if (!inputData.size()) return std::string();

  // decode header
  std::string header = inputData.substr(0, 514);
  inputData = inputData.substr(514);

  // std::cout << header.size() << std::endl;
  unsigned int probCounter = 0;
  for (int i = 0; i < header.size(); i += 2)
  {
    char* headerPtr = const_cast<char*>(&header[i]);
    unsigned short* codedProb = (unsigned short*)((char*)headerPtr);
    probabilitiesHighD[probCounter++] = *codedProb;
    if (probCounter < 257) probabilitiesLowD[probCounter] = *codedProb;
  }
  // end of decoding of header


  // if (inputData.size() != 0 && (inputData.size() % 2 == 1)) inputData.push_back(0);

  bitstring data;
  for (size_t i = 0; i < inputData.size(); ++i)
  {
    unsigned int val = (int)((unsigned char)inputData[i]);
    bitstring tmp;
    for (int j = 0; j < 8; ++j) tmp.push_back(false);
    tmp.ChangeValue(val);
    data += tmp;
  }
  data.Generate();

  bitstring code;
  for (counter = 0; counter < 16; ++counter)
    code.push_back(data[counter]);
  code.Generate();
  codeVal = code.Value();

  unsigned short int lowCount;
  unsigned short int highCount;

  while (counter < data.size())
  {
    unsigned short int scale = probabilitiesHighD[256];
    unsigned char c;
    count = (short int)((((long)(codeVal - low) + 1) * scale - 1) / long((high - low) + 1));

    int n = 0;
    for (n = 0; n < 257; ++n)
    {
      if (count >= probabilitiesLowD[n] && count < probabilitiesHighD[n])
      {
        lowCount = probabilitiesLowD[n];
        highCount = probabilitiesHighD[n];
        c = (unsigned char)n;
        break;
      }
    }
    if (n == 256) break;

    long range = ((long)(high - low) + 1);
    high = low + (unsigned short int)((range * highCount) / scale - 1);
    low = low + (unsigned short int)((range * lowCount) / scale);

    while (true)
    {
      if ((high < 0x8000) || (low >= 0x8000));
      else if ((low >= 0x4000) && (high < 0xC000))
      {
        codeVal ^= 0x4000;
        low &= 0x3fff;
        high |= 0x4000;
      }
      else break;
      low <<= 1;
      high <<= 1;
      high |= 1;
      codeVal <<= 1;
      if (counter < data.size() && data[counter]) codeVal += 0x1;
      counter++;
    }
    output.push_back(c);
  }

  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> duration = now - then;
  std::cout << "Decoding Time Taken : " << duration.count() * 1000.0 << " milliseconds" << std::endl;

  return output;
}
