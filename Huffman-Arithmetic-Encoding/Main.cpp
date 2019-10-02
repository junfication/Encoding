// Huffman-Arithmetic-Encoding.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include "HuffmanEncoding.h"
#include "ArithmeticEncoding.h"
#include "Args.h"
#include "Connection.h"

int main(int argc, char** argv)
{
  ArgumentsParser ap;
  if (argc != 7)
  {
    std::cout << "Wrong number of arguments" << std::endl;
    return -1;
  }
  else
  {
    if (!ap.ParseArguments(argv))
    {
      std::cout << "Wrong input of arguments" << std::endl;
      return -1;
    }
    else
    {
      if (ap.mode == MODE::LOCAL_CODING)
      {
        if (ap.operation == TYPE::ARITHMETIC_ENCODE)
          ArithmeticEncoding(ap.input, ap.output);
        else if (ap.operation == TYPE::ARITHMETIC_DECODE)
          ArithmeticDecoding(ap.input, ap.output);
        else if (ap.operation == TYPE::HUFFMAN_DECODE)
          HuffmanDecoding(ap.input, ap.output);
        else if (ap.operation == TYPE::HUFFMAN_ENCODE)
          HuffmanEncoding(ap.input, ap.output);
      }
      else if (ap.mode == MODE::SENDER)
      {
        Sender(ap);
      }
      else if (ap.mode == MODE::RECIEVER)
      {
        Reciever(ap);
      }
      else 
        std::cout << "Error!" << std::endl; // this should not print out
      return 0;
    }

  }
  // Reading of input here
  // std::cout << "Hello World!\n";
  // HuffmanEncoding("pg2701.txt");
  // HuffmanDecoding("HuffmanEncoded.huf");
  // ArithmeticEncoding("pg2701.txt");
  // ArithmeticDecoding("ArithmeticEncoded.ari");

}
