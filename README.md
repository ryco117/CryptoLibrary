CryptoLibrary
=============
is a very experimental C++ library written to a consolidate and improve the cryptographic functions I've written through various early projects.
The curve25519_donna code is taken from https://code.google.com/archive/p/curve25519-donna/ but the rest is developed completely in-house, and thus subjected to less professional scrutiny. Just a warning.


How to build / install
======================
First ensure assembler 'nasm' and C library 'libscrypt' are installed. Then simply run `$ make` to build ./lib/libcryptolibrary.a and `# make install` to install library and include files to '/usr/local/' prefix.
If you are not building for a platform that supports AES-NI, or cannot find an assembler to support syntax, can force C++ AES routine by building as `$ make no-ni` or `$ make all` for both.
This results in creating a libcryptolibrary-no-ni.a library file.
