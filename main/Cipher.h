#pragma once

#include <string>

class Cipher {
public:
	Cipher();
	~Cipher() = default;
	void initTest();
	bool checkKeyAHRS();

private:
	std::string _id;
};

