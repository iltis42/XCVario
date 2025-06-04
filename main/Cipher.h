#pragma once

#include <string>

class Cipher {
public:
	Cipher();
	~Cipher() = default;
	bool checkKeyAHRS();

private:
	std::string _id;
};

