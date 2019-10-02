#include "Args.h"

bool ArgumentsParser::ParseArguments(char** args)
{
  const int numberOfArguments = 7;
  for (int i = 1; i < numberOfArguments; ++i)
  {
    std::string arg(args[i]);
    if (arg[0] != '-') return false;

    if (arg.size() == 2)
    {
      if (arg[1] == 'c')
        mode = MODE::LOCAL_CODING;
      else if (arg[1] == 's')
        mode = MODE::SENDER;
      else if (arg[1] == 'r')
        mode = MODE::RECIEVER;
      else if (arg[1] == 'i')
      {
        if (++i == numberOfArguments) return false;
        input = args[i];
      }
      else if (arg[1] == 'o')
      {
        if (++i == numberOfArguments) return false;
        output = args[i];
      }
      else if (arg[1] == 'h')
        operation = TYPE::HUFFMAN;
      else if (arg[1] == 'a')
        operation = TYPE::ARITHMETIC;
      else return false;
    }
    else if (arg.size() == 3)
    {
      if (arg[1] == 'h')
      {
        if (arg[2] == 'e')
          operation = TYPE::HUFFMAN_ENCODE;
        else if (arg[2] == 'd')
          operation = TYPE::HUFFMAN_DECODE;
        else return false;
      }
      else if (arg[1] == 'a')
      {
        if (arg[2] == 'e')
          operation = TYPE::ARITHMETIC_ENCODE;
        else if (arg[2] == 'd')
          operation = TYPE::ARITHMETIC_DECODE;
        else return false;
      }
      else return false;
    }
  }

  if (input.empty() || output.empty() || mode == MODE::NO_MODE || operation == TYPE::NO_TYPE)
    return false;
  if ((mode == MODE::LOCAL_CODING) && (operation == TYPE::ARITHMETIC || operation == TYPE::HUFFMAN))
    return false;
  if ((mode == MODE::SENDER || mode == MODE::RECIEVER) &&
      (operation == TYPE::ARITHMETIC_DECODE || operation == TYPE::ARITHMETIC_ENCODE || operation == TYPE::HUFFMAN_DECODE || operation == TYPE::HUFFMAN_ENCODE))
    return false;

  return true;
}
