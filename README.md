# Cripta
For creating a readable encrypted directory.

Pick a directory to encrypt, the program will turn it into a CRIPTA file.
The files inside are encrypted using aes-128 and their integrity is checked with md5.

Using the same application to open the file will list all contents inside the directory. Choosing a file will decrypt it and create it locally.

Requires OpenSSL
