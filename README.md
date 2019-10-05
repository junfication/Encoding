## Usage
### Server
app.exe -s -i input.txt -h -o output.txt  
-s (for sender)  
-i (input file name)  
-h (huffman can change to -a for arithmetic)  
-o (output file name)  

### Client
app.exe -r -i input.txt -h -o output.txt  
-r (for reciever)  
-i (input file name but does not matter for client side but still need for argument parser)  
-h (huffman can change to -a for arithmetic)  
-o (output file name)  

### Local  
app.exe -c -i input.txt -he -o output.txt  
-c (for local encode/decode)  
-i (input file name)  
-he (huffman encoding can change to -ae for arithmetic encoding, -ad for arithmetic decoding and -hd for huffman decoding)  
-o (output file name)  
