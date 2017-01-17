CXX=g++
CC=gcc

PREFIX=/usr/local

OBJECTS-BASE=base64.o curve25519-donna.o ecdh.o fortuna.o SecureFixedLengthArray.o SecureString.o RLWE.o
OBJECTS-NI=AES.o AES-NI.o $(OBJECTS-BASE)
OBJECTS-NO-NI=AES-NO-NI.o $(OBJECTS-BASE)

make: ./lib/libcryptolibrary.a
	mkdir -p ./include
	cp -f ./src/*.h ./include/

no-ni: ./lib/libcryptolibrary-no-ni.a
	mkdir -p ./include

all: ./lib/libcryptolibrary.a ./lib/libcryptolibrary-no-ni.a tests
	mkdir -p ./include
	cp -f ./src/*.h ./include/

tests: ./lib/libcryptolibrary.a ./lib/libcryptolibrary-no-ni.a
	mkdir -p ./bin
	$(CXX) -o ./bin/test_AES -O0 ./src/test_AES.cpp -fPIC -std=c++11 -I./src -L./lib -lcryptolibrary
	$(CXX) -o ./bin/test_SecureString -O2 ./src/test_SecureString.cpp -fPIC -std=c++11 -I./src -L./lib -lcryptolibrary
	$(CXX) -o ./bin/test_RLWE ./src/test_RLWE.cpp -fPIC -std=c++11 -I./src -L./lib -lcryptolibrary -lscrypt
	$(CXX) -o ./bin/test_Fortuna -O2 ./src/test_Fortuna.cpp -fPIC -std=c++11 -I./src -L./lib -lcryptolibrary -lscrypt

install:
	mkdir -p $(PREFIX)/include/crypto
	cp -f ./lib/* $(PREFIX)/lib/
	cp -f ./include/* $(PREFIX)/include/crypto/

clean:
	rm -f *.o ./lib/*.a

./lib/libcryptolibrary.a: $(OBJECTS-NI)
	mkdir -p ./lib
	ar rvs ./lib/libcryptolibrary.a $(OBJECTS-NI)

./lib/libcryptolibrary-no-ni.a: $(OBJECTS-NO-NI)
	mkdir -p ./lib
	ar rvs ./lib/libcryptolibrary-no-ni.a $(OBJECTS-NO-NI)

./AES-NI.o: ./src/AES.asm
	nasm -f elf64 -o AES-NI.o ./src/AES.asm

./AES.o: ./src/AES.cpp
	$(CXX) -std=c++11 -c -o ./AES.o -O2 ./src/AES.cpp -fPIC

./AES-NO-NI.o: ./src/AES.cpp
	$(CXX) -std=c++11 -c -o ./AES-NO-NI.o -O2 ./src/AES.cpp -DNO_NI -fPIC

./base64.o: ./src/Base64.cpp
	$(CXX) -std=c++11 -c -o ./base64.o -O2 ./src/Base64.cpp -fPIC

./curve25519-donna.o: ./src/curve25519-donna.c
	$(CC) -c -o ./curve25519-donna.o -O2 ./src/curve25519-donna.c -fPIC

./ecdh.o: ./src/ecdh.cpp
	$(CXX) -c -o ./ecdh.o -O2 ./src/ecdh.cpp -fPIC -I./src

./fortuna.o: ./src/Fortuna.cpp
	$(CXX) --std=c++11 --static -c -o ./fortuna.o -O2 ./src/Fortuna.cpp -lscrypt -fPIC -I./src/

./SecureFixedLengthArray.o: ./src/SecureFixedLengthArray.cpp
	$(CXX) -c --std=c++11 -o ./SecureFixedLengthArray.o -O2 ./src/SecureFixedLengthArray.cpp -fPIC -I./src

./SecureString.o: ./src/SecureString.cpp
	$(CXX) -c --std=c++11 -o ./SecureString.o -O2 ./src/SecureString.cpp -fPIC -I./src

./RLWE.o : ./src/RLWE.cpp
	$(CXX) -c --std=c++11 --static -o ./RLWE.o ./src/RLWE.cpp -fPIC -I./src -lscrypt
