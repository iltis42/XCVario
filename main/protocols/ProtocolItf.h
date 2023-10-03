#pragma once

// Supported protocol id's
enum protocol_nt {
	NO_ONE,     // Disable
	NMEA,
	ANEMOI
};

// Generic protocol state machine
enum gen_state_t {
	START_TOKEN,
	HEADER,
	PAYLOAD,
	COMPLETE,
	CHECK_OK,
	CHECK_FAILED,
	STOP_TOKEN,
	ABORT
};

// Protocol parser interface
class ProtocolItf {
public:
	virtual ~ProtocolItf() {}

public:
	// API
	virtual protocol_nt getProtocolId() const = 0;
	virtual gen_state_t nextByte(const char c) = 0;
	gen_state_t getState() const { return _state; }
	const char* getBuffer() { return _framebuffer; }
	int getLength() const { return _pos; }
	bool isPassThrough() const { return _pass_through; }
	virtual bool isNMEA() const { return true; }

protected:
	void reset() { _pos = 0; _len = 0; _crc = 0; }
	void push(char c) { _framebuffer[_pos++] = c; incrCRC(c); }
	virtual void incrCRC(const char c) = 0;
	enum gen_state_t _state = START_TOKEN;
	bool _pass_through = false;
	char _framebuffer[128];
	int  _pos = 0;
	int  _len = 0;
	char _crc = 0; // checksum
};

