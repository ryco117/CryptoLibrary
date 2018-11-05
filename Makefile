CXX=g++
CC=gcc
OPTIMIZATION=-O2
TEST_LIB=cryptolibrary

PREFIX=/usr/local

OBJECTS-BASE=bin/base64.o bin/curve25519-donna.o bin/ecdh.o bin/fortuna.o bin/SecureFixedLengthArray.o bin/SecureString.o bin/RLWE.o
OBJECTS-NI=bin/AES.o bin/AES-NI.o $(OBJECTS-BASE)
OBJECTS-NO-NI=bin/AES-NO-NI.o $(OBJECTS-BASE)
TESTS=./bin/test_AES ./bin/test_SecureString ./bin/test_RLWE ./bin/test_Fortuna

make: ./lib/libcryptolibrary.a
	mkdir -p ./include
	cp -f ./src/*.h ./include/

no-ni: ./lib/libcryptolibrary-no-ni.a
	mkdir -p ./include
	cp -f ./src/*.h ./include/

all: ./lib/libcryptolibrary.a ./lib/libcryptolibrary-no-ni.a tests
	mkdir -p ./include
	cp -f ./src/*.h ./include/

tests: ./lib/lib$(TEST_LIB).a
	mkdir -p ./bin
	$(CXX) -o ./bin/test_AES $(OPTIMIZATION) ./src/test_AES.cpp -fPIC -std=c++11 -I./src -L./lib -l$(TEST_LIB)
	$(CXX) -o ./bin/test_SecureString $(OPTIMIZATION) ./src/test_SecureString.cpp -fPIC -std=c++11 -I./src -L./lib -l$(TEST_LIB)
	$(CXX) -o ./bin/test_RLWE $(OPTIMIZATION) ./src/test_RLWE.cpp -fPIC -std=c++11 -I./src -L./lib -l$(TEST_LIB) -lscrypt
	$(CXX) -o ./bin/test_Fortuna $(OPTIMIZATION) ./src/test_Fortuna.cpp -fPIC -std=c++11 -I./src -L./lib -l$(TEST_LIB) -lscrypt

install:
	mkdir -p $(PREFIX)/include/crypto
	cp -f ./lib/* $(PREFIX)/lib/
	cp -f ./include/* $(PREFIX)/include/crypto/

clean:
	rm -f ./bin/*.o ./include/*.h ./lib/*.a $(TESTS)

./lib/libcryptolibrary.a: $(OBJECTS-NI)
	mkdir -p ./lib
	ar rvs ./lib/libcryptolibrary.a $(OBJECTS-NI)

./lib/libcryptolibrary-no-ni.a: $(OBJECTS-NO-NI)
	mkdir -p ./lib
	ar rvs ./lib/libcryptolibrary-no-ni.a $(OBJECTS-NO-NI)

./bin/AES-NI.o: ./src/AES.asm
	nasm -f elf64 -o bin/AES-NI.o ./src/AES.asm

./bin/AES.o: ./src/AES.cpp ./bin/AES-NI.o
	$(CXX) -std=c++11 -c -o ./bin/AES.o $(OPTIMIZATION) ./bin/AES-NI.o ./src/AES.cpp -fPIC

./bin/AES-NO-NI.o: ./src/AES.cpp
	$(CXX) -std=c++11 -c -o ./bin/AES-NO-NI.o $(OPTIMIZATION) ./src/AES.cpp -DNO_NI -fPIC

./bin/base64.o: ./src/Base64.cpp
	$(CXX) -std=c++11 -c -o ./bin/base64.o $(OPTIMIZATION) ./src/Base64.cpp -fPIC

./bin/curve25519-donna.o: ./src/curve25519-donna.c
	$(CC) -c -o ./bin/curve25519-donna.o $(OPTIMIZATION) ./src/curve25519-donna.c -fPIC

./bin/ecdh.o: ./src/ecdh.cpp
	$(CXX) -c -o ./bin/ecdh.o $(OPTIMIZATION) ./src/ecdh.cpp -fPIC -I./src

./bin/fortuna.o: ./src/Fortuna.cpp
	$(CXX) --std=c++11 --static -c -o ./bin/fortuna.o $(OPTIMIZATION) ./src/Fortuna.cpp -lscrypt -fPIC -I./src/

./bin/SecureFixedLengthArray.o: ./src/SecureFixedLengthArray.cpp
	$(CXX) -c --std=c++11 -o ./bin/SecureFixedLengthArray.o $(OPTIMIZATION) ./src/SecureFixedLengthArray.cpp -fPIC -I./src

./bin/SecureString.o: ./src/SecureString.cpp
	$(CXX) -c --std=c++11 -o ./bin/SecureString.o $(OPTIMIZATION) ./src/SecureString.cpp -fPIC -I./src

./bin/RLWE.o : ./src/RLWE.cpp
	$(CXX) -c --std=c++11 --static $(OPTIMIZATION) -o ./bin/RLWE.o ./src/RLWE.cpp -fPIC -I./src -lscrypt
