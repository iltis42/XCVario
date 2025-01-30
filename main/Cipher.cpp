#include "Cipher.h"
#include "SetupNG.h"
#include "logdef.h"
#include "sensor.h"
#include <esp_mac.h>
#include <miniz.h>

std::string Cipher::_id;

std::string Cipher::Encrypt(std::string key, std::string plaintext) {
	// std::string to hold our encrypted text
	std::string encrypted;
	// Format key
	FormatKey(key, plaintext);
	// Begin encryption
	for (int i = 0; i < plaintext.length(); i++) {
		int keyDistance = CalculateDistance(key.at(i), 'A'); // Key's current char distance from 'a'
		char EncryptedChar = ShiftChar(plaintext.at(i), keyDistance); // Encrypt the plaintext char shifting the plaintext char by the keyDistance
		encrypted.push_back(EncryptedChar); // Add char to encrypted string
	}
	FormatEncrypted(encrypted);
	return encrypted; // Return the encrypted string
}

std::string Cipher::Decrypt(std::string key, std::string plaintext) {
	// std::string to hold our encrypted text
	std::string encrypted;
	// Format key
	FormatKey(key, plaintext);
	// Begin encryption
	for (int i = 0; i < plaintext.length(); i++) {
		int keyDistance = CalculateDistance(key.at(i), 'A'); // Key's current char distance from 'a'
		char EncryptedChar = ShiftCharBack(plaintext.at(i), keyDistance); // Encrypt the plaintext char shifting the plaintext char by the keyDistance
		encrypted.push_back(EncryptedChar); // Add char to encrypted string
	}
	FormatEncrypted(encrypted);
	return encrypted; // Return the encrypted string
}


int Cipher::CalculateDistance(char a, char b) {
	// ESP_LOGI(FNAME,"CalculateDistance a:%c b:%c  ret:%d", a, b, abs(a - b));
	return abs(a - b);
}

char Cipher::ShiftChar(char a, int keyDistance) {
	// ESP_LOGI(FNAME,"ShiftChar %c, %d", a, keyDistance );
	for (int i = 0; i < keyDistance; i++) {
		a += 1; // shift our char once
		if( a < 65 && a > 57 )
			a+=7;  // skip ;:<...
		if (a > 90) // Check if we've gone past z
			a = 48; // Set our char back to a
	}
	// ESP_LOGI(FNAME,"ShiftChar ret: %c ", a );
	return a;
}

char Cipher::ShiftCharBack(char a, int keyDistance) {
	// ESP_LOGI(FNAME,"ShiftCharBack %c, %d", a, keyDistance );
	for (int i = 0; i < keyDistance; i++) {
		a -= 1; // shift our char once
		if( a < 65 && a > 57 )
			a-=7;  // skip ;:<...
		if (a < 48) // Check if we've gone before a
			a = 90; // Set our char back to z
	}
	// ESP_LOGI(FNAME,"ShiftCharBack ret: %c ", a );
	return a;
}

void Cipher::FormatKey(std::string& key, std::string plaintext) {
	// append key length to plaintext's size
	// note: key may end up larger than plaintext here, but will be trimmed in the next if statement
	if (key.length() < plaintext.length()) {
		while (key.length() < plaintext.length())
			key.append(key); // append key onto itself until >= plaintext.length()
	}
	// shrink key length to plaintext's size
	if (key.length() > plaintext.length())
		key.erase(key.length(), std::string::npos);
}

void Cipher::FormatEncrypted(std::string& encrypted) {
	std::locale loc;
	for (int i = 0; i < encrypted.length(); i++)
		encrypted.at(i) = std::toupper(encrypted.at(i), loc); // Make all character uppercase
}

void Cipher::begin()
{
	uint8_t mac[6];
	esp_efuse_mac_get_default(mac);
	char id[6];
	sprintf( id,"%04d", (int)(mz_crc32(0L, mac, 6) % 10000) );
	ESP_LOGI(FNAME,"Cipher::id() returns: %s", id );
	_id = std::string( id );
}

bool Cipher::init(){
	std::string encid = Cipher::Encrypt(CIPHER_KEY, _id );
	ESP_LOGI(FNAME,"init() Encrypted ID %s", encid.c_str() );
	ahrs_licence_dig1.set( encid[0]-'0' );
	ahrs_licence_dig2.set( encid[1]-'0' );
	ahrs_licence_dig3.set( encid[2]-'0' );
	ahrs_licence_dig4.set( encid[3]-'0' );
	std::string decid = Cipher::Decrypt(CIPHER_KEY, encid );
	gflags.ahrsKeyValid = (_id == decid);
	ESP_LOGI(FNAME,"init() ID/DECID %s == %s returns %d", _id.c_str(), decid.c_str(), gflags.ahrsKeyValid );
	return gflags.ahrsKeyValid;
}

const char * Cipher::id(){
	return( _id.c_str() );
}

bool Cipher::checkKeyAHRS(){

	std::string key;
	key += char(ahrs_licence_dig1.get()+'0');
	key += char(ahrs_licence_dig2.get()+'0');
	key += char(ahrs_licence_dig3.get()+'0');
	key += char(ahrs_licence_dig4.get()+'0');
	std::string decid = Cipher::Decrypt(CIPHER_KEY, key );
	gflags.ahrsKeyValid = (_id == decid);
	ESP_LOGI(FNAME,"checkKeyAHRS() ID/KEY/DECID %s %s %s returns %d", _id.c_str(), key.c_str(), decid.c_str(), gflags.ahrsKeyValid );
	return gflags.ahrsKeyValid;
}
