#include "Connection.h"

#define LENGTH 500 // per packet size

void Sender(const ArgumentsParser& ap)
{
  char hostname[512];
  SOCKET listenSocket;
  SOCKET dataSocket;

  WSADATA data;

  SecureZeroMemory(&data, sizeof(data));

  int result = WSAStartup(MAKEWORD(2, 2), &data);
  if (result)
  {
    std::cout << "WSAStartup failed" << std::endl;
    return;
  }

  int major = LOBYTE(data.wVersion);
  int minor = HIBYTE(data.wVersion);

  printf("WinSock version : %d.%d\n", major, minor);

  std::string port_input;
  listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (listenSocket == INVALID_SOCKET)
  {
    std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return;
  }

  result = gethostname(hostname, LENGTH);

  if (result == SOCKET_ERROR)
  {
    std::cout << "Gethostname failed with error: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return;
  }

  addrinfo* result_addr, hints;
  SecureZeroMemory(&hints, sizeof(hints));

  hints.ai_family = AF_INET; // ipv4
  hints.ai_socktype = SOCK_STREAM; // using reliable byte stream TCP
  hints.ai_protocol = IPPROTO_TCP; // using TCP
  hints.ai_flags = AI_PASSIVE; // telling the socket that it will be use for binding later

  std::cout << "Please enter port number" << std::endl;

  std::getline(std::cin, port_input);

  std::cout << std::endl;

  result = getaddrinfo(hostname, port_input.c_str(), &hints, &result_addr);

  if (result != 0)
  {
    std::cout << "Getaddrinfo failed with error: " << result << std::endl;
    WSACleanup();
    return;
  }

  std::cout << "Hostname: " << hostname << std::endl;

  sockaddr_in* addr;

  addr = (sockaddr_in*)result_addr->ai_addr;
  void* ip = &addr->sin_addr;
  char ip_addr[LENGTH];
  inet_ntop(AF_INET, ip, ip_addr, INET_ADDRSTRLEN);

  std::cout << "IP Address: " << ip_addr << std::endl;
  std::cout << "Port Number: " << port_input.c_str() << std::endl;

  if (bind(listenSocket, (SOCKADDR*)addr, sizeof(*addr)) == SOCKET_ERROR)
  {
    std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
    closesocket(listenSocket);
    WSACleanup();
    return;
  }

  freeaddrinfo(result_addr);

  // listening on the listen socket
  if (listen(listenSocket, 1) == SOCKET_ERROR)
  {
    std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
    closesocket(listenSocket);
    WSACleanup();
    return;
  }

  printf("Waiting for client to connect...\n");

  dataSocket = accept(listenSocket, NULL, NULL);

  if (dataSocket == INVALID_SOCKET)
  {
    std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
    closesocket(listenSocket);
    WSACleanup();
    return;
  }

  closesocket(listenSocket);

  std::cout << "Connected.\n\n";

  char recvbuf[LENGTH];
  int sendResult;

  std::string message;
  std::string fullData;
  
  std::ifstream input(ap.input, std::ios::binary);
  if (!input.is_open())
  {
    std::cout << "No file is opened" << std::endl;
    return;
  }

  std::string inputData(std::istreambuf_iterator<char>(input), {});

  if (ap.operation == TYPE::HUFFMAN)
    message = HuffmanEncoderHelper(inputData);
  else if (ap.operation == TYPE::ARITHMETIC)
    message = ArithmeticEncodingHelper(inputData);

  std::ofstream file(ap.output, std::ios::binary);
  file << message;
  file.close();

  unsigned long long iterations = 0;
  iterations = message.size() / LENGTH;
  if ((message.size() % LENGTH) != 0) ++iterations;
  
  // send first packet of the packet size
  std::string packetSize;
  packetSize.resize(sizeof(unsigned long long));
  unsigned long long* tmp = (unsigned long long*)((void*)(const_cast<char*>(packetSize.c_str())));
  *tmp = iterations;

  sendResult = send(dataSocket, packetSize.c_str(), packetSize.size(), 0);

  if (sendResult == SOCKET_ERROR)
  {
    std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
    closesocket(dataSocket);
    WSACleanup();
    return;
  }

  ZeroMemory(recvbuf, LENGTH);

  result = recv(dataSocket, recvbuf, LENGTH, 0);

  if (!result)
  {
    std::cout << "recv function failed with error: " << WSAGetLastError();
    closesocket(dataSocket);
    WSACleanup();
    return;
  }

  std::string ack(recvbuf);
  if (ack != "YES")
  {
    std::cout << "no ack" << std::endl;
    closesocket(dataSocket);
    WSACleanup();
    return;
  }
  else
  {
    std::cout << "starting to send data over" << std::endl;
  }

  bool sending = true;
  unsigned long long counter = 0;
  unsigned long long startIndex = 0;

  do
  {
    if (sending)
    {
      // split message here
      std::string msg;

      if (startIndex + LENGTH < message.size())
      {
        msg = message.substr(startIndex, LENGTH);
        startIndex += LENGTH;
      }
      else msg = message.substr(startIndex);

      sendResult = send(dataSocket, msg.c_str(), msg.size(), 0);

      if (sendResult == SOCKET_ERROR)
      {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(dataSocket);
        WSACleanup();
        return;
      }

      counter = counter + 1;

      ZeroMemory(recvbuf, LENGTH);

      result = recv(dataSocket, recvbuf, LENGTH, 0);

      if (!result)
      {
        std::cout << "recv function failed with error: " << WSAGetLastError();
        closesocket(dataSocket);
        WSACleanup();
        return;
      }

      std::string ack(recvbuf);
      if (ack != "YES")
      {
        std::cout << "no ack pkt" << std::endl;
        closesocket(dataSocket);
        WSACleanup();
        return;
      }

      if (counter == iterations)
      {
        counter = 0;
        sending = false;
        ack = std::string("YES");
      }
    }
    else
    {
      ZeroMemory(recvbuf, LENGTH);

      result = recv(dataSocket, recvbuf, LENGTH, 0);

      if (!result)
      {
        std::cout << "recv function failed with error: " << WSAGetLastError();
        closesocket(dataSocket);
        WSACleanup();
        return;
      }

      std::string data;
      data.resize(result);
      for (int i = 0; i < result; ++i) data[i] = recvbuf[i];
      fullData += data;

      
      result = send(dataSocket, ack.c_str(), ack.size(), 0);

      if (result == SOCKET_ERROR)
      {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(dataSocket);
        WSACleanup();
        return;
      }

      ++counter;
      if (counter == iterations)
      {
        if (fullData.size() != message.size()) std::cout << "size does not match" << std::endl;
        else
        {
          unsigned long long i = 0;
          for(; i < message.size(); ++i)
            if (message[i] != fullData[i])
            {
              std::cout << "output does not match" << std::endl;
              break;
            }
          if (i == message.size()) std::cout << "output matches" << std::endl;
        }
        break;
      }
    }

  } while (true);

  // close data sock
  closesocket(dataSocket);

  // clean up WSAdata
  WSACleanup();

  return;

}

void Reciever(const ArgumentsParser& ap)
{
  WSADATA data;
  SecureZeroMemory(&data, sizeof(data));
  int result = WSAStartup(MAKEWORD(2, 2), &data);

  if (result != NO_ERROR)
  {
    std::cout << "WSAStartup function failed with error: " << result << std::endl;
    return;
  }

  // string to store the user input of the port number
  std::string portInput;
  // string to  store the user input of the ip
  std::string ipInput;

  addrinfo* result_addr = nullptr;
  addrinfo* ptr = nullptr;
  addrinfo   hints;

  SecureZeroMemory(&hints, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  std::cout << "Please input server ip address...\n";

  std::getline(std::cin, ipInput);

  std::cout << "Please input server port number...\n";

  std::getline(std::cin, portInput);

  result = getaddrinfo(ipInput.c_str(), portInput.c_str(), &hints, &result_addr);

  if (result != 0)
  {
    std::cout << "Getaddrinfo failed with error: " << result << std::endl;
    WSACleanup();
    return;
  }

  ptr = result_addr;
  SOCKET ConnectSock;

  // loop through the result_addr to search for a address to connect to
  for (ptr = result_addr; ptr != nullptr; ptr = ptr->ai_next)
  {
    // create a connect socket
    ConnectSock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    // if creation fails
    if (ConnectSock == INVALID_SOCKET)
    {
      std::cout << "socket function failed with error: " << WSAGetLastError() << std::endl;
      WSACleanup();
      return;
    }

    // connect the connect socket to the ptr address
    result = connect(ConnectSock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));

    // if cannot connect, close the connect socket and continue to next loop
    if (result == SOCKET_ERROR)
    {
      closesocket(ConnectSock);
      ConnectSock = INVALID_SOCKET;
      continue;
    }
    // if connected, break from for loop
    break;
  }

  freeaddrinfo(result_addr);

  std::cout << "Connected.\n\n";

  std::string sendbuf;
  char recvbuf[LENGTH];

  ZeroMemory(recvbuf, LENGTH);

  result = recv(ConnectSock, recvbuf, LENGTH, 0);

  if (!result)
  {
    std::cout << "recv function failed with error: " << WSAGetLastError();
    closesocket(ConnectSock);
    WSACleanup();
    return;
  }

  unsigned long long packetSize = *(unsigned long long*)((void*)recvbuf);
  std::cout << "Total Number of Packet is : " << packetSize << std::endl;

  ZeroMemory(recvbuf, LENGTH);

  std::string fullData;
  std::string decoded;
  std::string encoded;

  // send ack to other party
  std::string ack("YES");

  result = send(ConnectSock, ack.c_str(), ack.size(), 0);

  if (result == SOCKET_ERROR)
  {
    std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
    closesocket(ConnectSock);
    WSACleanup();
    return;
  }

  bool recieving = true;
  unsigned long long counter = 0;
  unsigned long long startIndex = 0;


  do
  {
    if (recieving)
    {
      ZeroMemory(recvbuf, LENGTH);

      result = recv(ConnectSock, recvbuf, LENGTH, 0);

      if (!result)
      {
        std::cout << "recv function failed with error: " << WSAGetLastError();
        closesocket(ConnectSock);
        WSACleanup();
        return;
      }

      std::string data;
      data.resize(result);
      for (int i = 0; i < result; ++i) data[i] = recvbuf[i];
      fullData += data;

      result = send(ConnectSock, ack.c_str(), ack.size(), 0);

      if (result == SOCKET_ERROR)
      {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSock);
        WSACleanup();
        return;
      }

      ++counter;
      if (counter == packetSize)
      {
        recieving = false;
        counter = 0;
        // decode and encode here
        if (ap.operation == TYPE::HUFFMAN)
        {
          decoded = HuffmanDecoderHelper(fullData);
          encoded = HuffmanEncoderHelper(decoded);
        }
        else if (ap.operation == TYPE::ARITHMETIC)
        {
          decoded = ArithmeticDecodingHelper(fullData);
          encoded = ArithmeticEncodingHelper(decoded);
        }
        std::ofstream file(ap.output, std::ios::binary);
        file << encoded;
        file.close();
      }
    }
    else // decode then encode then send
    {
      // split message here
      std::string msg;

      if (startIndex + LENGTH < encoded.size())
      {
        msg = encoded.substr(startIndex, LENGTH);
        startIndex += LENGTH;
      }
      else msg = encoded.substr(startIndex);

      result = send(ConnectSock, msg.c_str(), msg.size(), 0);

      if (result == SOCKET_ERROR)
      {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSock);
        WSACleanup();
        return;
      }

      counter = counter + 1;

      ZeroMemory(recvbuf, LENGTH);

      result = recv(ConnectSock, recvbuf, LENGTH, 0);

      if (!result)
      {
        std::cout << "recv function failed with error: " << WSAGetLastError();
        closesocket(ConnectSock);
        WSACleanup();
        return;
      }

      std::string tmpAck(recvbuf);
      if (tmpAck != "YES")
      {
        std::cout << "no ack pkt" << std::endl;
        closesocket(ConnectSock);
        WSACleanup();
        return;
      }

      if (counter == packetSize)
        break;
    }

  } while (true);

  result = closesocket(ConnectSock);

  if (result == SOCKET_ERROR)
  {
    std::cout << "closesocket function failed with error: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return;
  }

  WSACleanup();

  return;
}
