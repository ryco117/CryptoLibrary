make:
	nasm -f elf64 -o AES-NI.o AES.asm

	g++ --std=c++11 -o test_AES test_AES.cpp AES-NI.o AES.cpp SecureFixedLengthArray.cpp Base64.cpp -I./ -w -fpermissive
	g++ --std=c++11 -o test_RLWE test_RLWE.cpp RLWE.cpp AES-NI.o AES.cpp SecureFixedLengthArray.cpp Base64.cpp Fortuna.cpp -I./ -lscrypt -Wall
