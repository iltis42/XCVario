#ifndef CIPHEREV_H
#define CIPHEREV_H
#include <stdlib.h> // abs
#include <string> // std::string
#include <locale> // std::locale, std::toupper
#include "cipherkey.h"


class Cipher {
public:
	static void begin();
	static std::string Encrypt(std::string key, std::string text);
	static std::string Decrypt(std::string key, std::string text);
	static bool checkKeyAHRS();
	static bool init();
	static const char* id();
private:
	static void FormatKey(std::string&, std::string);
	static void FormatEncrypted(std::string&);
	static char ShiftChar(char, int);
	static char ShiftCharBack(char, int);
	static int CalculateDistance(char, char);
	static std::string _id;
};

#endif
