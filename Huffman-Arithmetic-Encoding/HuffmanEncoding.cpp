#include "HuffmanEncoding.h"

void HuffmanEncoding(const std::string& fn)
{
  // tmp text for debugging purposes;
  std::ifstream input(fn, std::ios::binary);
  if (!input.is_open())
  {
    std::cout << "No file is opened" << std::endl;
    return;
  }
  std::string inputData(std::istreambuf_iterator<char>(input), {});

  auto then = std::chrono::system_clock::now();

  // Generate histogram
  std::map<char, Node> histogram;
  for (const char& c : inputData)
    ++histogram[c].weight;

  // create min heap / tree
  auto cmp = [](Node* l, Node* r) {return l->weight > r->weight; };
  std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> tree(cmp);

  // push leaf nodes
  for (auto& h : histogram)
  {
    Node* n = new Node();
    n->symbol = h.first;
    n->weight = h.second.weight;
    tree.push(n);
  }

  // push eof leaf node
  Node* n = new Node();
  n->eof = true;
  n->weight = 1;
  tree.push(n);

  // Create Tree
  while (tree.size() != 1)
  {
    Node* l = tree.top();
    tree.pop();
    Node* r = tree.top();
    tree.pop();
    Node* merge = new Node();
    merge->left = l;
    merge->right = r;
    merge->weight = l->weight + r->weight;
    tree.push(merge);
  }

  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> duration = now - then;
  std::cout << "Time Taken : " << duration.count() * 1000.0 << " milliseconds" << std::endl;

  Node* top = tree.top();
  tree.pop();

  // Get Table of mapping
  auto table = top->GenerateTable();
  delete top;

  auto compare = [](std::pair<char, bitstring> l, std::pair<char, bitstring> r) {return l.second.size() < r.second.size(); };
  std::sort(table.begin(), table.end(), compare);

  // Canonical table
  std::map<char, bitstring> canonicalTable;
  bitstring eofBits;

  auto start = table.begin();
  unsigned sz = start->second.size();
  unsigned int val = 0x0;
  start->second.ChangeValue(val);
  canonicalTable[start->first] = start->second;
  if (start->second.eof) eofBits = start->second;
  std::string header;
  // push header size
  header.push_back((unsigned char)table.size());
  header.push_back(start->first);
  header.push_back((char)start->second.size());
  unsigned int oldSz = sz;
  unsigned int eofSz = 0;

  for (++start; start != table.end(); ++start)
  {
    oldSz = sz;
    val += 0x1;
    unsigned int diff = start->second.size() - sz;
    while (diff)
    {
      sz = start->second.size();
      val = val << 1;
      --diff;
    }
    start->second.ChangeValue(val);
    canonicalTable[start->first] = start->second;
    if (start->second.eof)
      eofBits = start->second;
    header.push_back(start->first);
    header.push_back((char)(start->second.size() - oldSz));
  }

  // Debugging
  std::cout << "Table : " << std::endl;
  for (auto& t : table)
  {
    if(!t.second.eof)
      std::cout << t.first <<" " << t.second.Value() << " " << t.second.String() << std::endl;
    else std::cout << "EOF" << " " << t.second.Value() << " " << t.second.String() << std::endl;
  }
  // End of Debugging

  // Huffman Compression
  bitstring data;
  for (auto& c : inputData)
    data += canonicalTable[c];
  data += eofBits;
  data.Generate();

  // Debugging of the bitstring output
  std::cout << data.String() << std::endl;
  // End of Debugging
  std::string output = data.Output();

  // Debugging compressed chars
  for (auto o : header) std::cout << (int)((unsigned char)o) << " ";
  std::cout << std::endl;
  // End of Debugging
    // Debugging compressed chars
  for (auto o : output) std::cout << (int)((unsigned char)o) << " ";
  std::cout << std::endl;
  // End of Debugging

  output = header + output;

  std::ofstream file("test.huf", std::ios::binary);
  file << output;
  file.close();


}

void HuffmanDecoding(const std::string& fn)
{
  std::ifstream input(fn, std::ios::binary);
  if (!input.is_open())
  {
    std::cout << "No file is opened" << std::endl;
    return;
  }
  std::string inputData(std::istreambuf_iterator<char>(input), {});
  std::cout << std::endl;
  size_t pos = (size_t)(((unsigned char)inputData[0]) * 2);
  std::string header = inputData.substr(1, pos);
  std::string encodedData = inputData.substr(pos + 1);

  // Debug header and encoded Data
  for (auto& c : header) std::cout << (int)((unsigned char)c) << " ";
  std::cout << std::endl;
  for (auto& c : encodedData) std::cout << (int)((unsigned char)c) << " ";
  std::cout << std::endl;
  // end of debug
  
  // construct canonical table
  std::map<bitstring, char> canonicalTable;
  if (header.size() >= 2)
  {
    unsigned i = 0;
    unsigned int val = 0x0;
    unsigned int size = 0;
    bitstring bs;
    size = (unsigned int)((unsigned char)header[1]);
    for (unsigned int j = 0; j < size; ++j)
      bs.push_back(false);
    bs.Generate();
    canonicalTable[bs] = header[0];

    for (i += 2; i < header.size(); i += 2)
    {
      val += 0x1;
      unsigned int symbolSize = (unsigned int)((unsigned char)header[i + 1]);
      bitstring bs;
      while (symbolSize != 0)
      {
        val = val << 1;
        ++size;
        --symbolSize;
      }
      for (unsigned int j = 0; j < size; ++j)
        bs.push_back(false);
      bs.ChangeValue(val);
      if (header[i] == 0) bs.eof = true;
      canonicalTable[bs] = header[i];
      
    }
  }
  std::cout << "Table : " << std::endl;
  for (auto& t : canonicalTable)
  {
    if (!t.first.eof)
      std::cout << t.second << " " << t.first.Value() << " " << t.first.String() << std::endl;
    else std::cout << "EOF" << " " << t.first.Value() << " " << t.first.String() << std::endl;
  }
  
  bitstring data;
  for (size_t i = 0; i < encodedData.size(); ++i)
  {
    unsigned int val = (int)((unsigned char)encodedData[i]);
    bitstring tmp;
    for (int j = 0; j < 8; ++j) tmp.push_back(false);
    tmp.ChangeValue(val);
    data += tmp;
  }
  data.Generate();
  std::cout << data.String() << std::endl;
    
  std::string decoded;
  bitstring code;
  for (unsigned int i = 0; i < data.size(); ++i)
  {
    code.push_back(data[i]);
    auto result = canonicalTable.find(code);
    if (result != canonicalTable.end())
    {
      if (result->first.eof) break;
      decoded.push_back(result->second);
      code.clear();
    }
  }
  std::ofstream file("huffmanDecoded.txt", std::ios::binary);
  file << decoded;
  file.close();
}

void Node::clear()
{
  if (left)
  {
    DeleteHelper(left);
    delete left;
  }
  if (right)
  {
    DeleteHelper(right);
    delete right;
  }
}

std::vector<std::pair<char, bitstring>> Node::GenerateTable()
{
  std::vector<std::pair<char, bitstring>> t;
  bitstring bs;
  if (!left && !right)
  {
    bs.push_back(false);
    bs.Generate();
    bs.eof = eof;
    t.push_back(std::pair<char, bitstring>(symbol, bs));
  }
  if (left)
  {
    bitstring l = bs;
    l.push_back(false);
    TableHelper(left, l, t);
    delete left;
  }
  if (right)
  {
    bitstring r = bs;
    r.push_back(true);
    TableHelper(right, r, t);
    delete right;
  }
  return t;
}

void Node::DeleteHelper(Node* n)
{
  if (n->left)
  {
    DeleteHelper(n->left);
    delete n->left;
  }
  if (n->right)
  {
    DeleteHelper(n->right);
    delete n->right;
  }
}

void Node::TableHelper(Node* n, bitstring bs, std::vector<std::pair<char, bitstring>>& t)
{
  if (!n->left && !n->right)
  {
    bs.Generate();
    bs.eof = n->eof;
    t.push_back(std::pair<char, bitstring>(n->symbol, bs));
  }
  if (n->left)
  {
    bitstring l = bs;
    l.push_back(false);
    TableHelper(n->left, l, t);
    delete n->left;
  }
  if (n->right)
  {
    bitstring r = bs;
    r.push_back(true);
    TableHelper(n->right, r, t);
    delete n->right;
  }
}
