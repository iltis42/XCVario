#pragma once

#include "ProtocolItf.h"

class Anemoi final : public ProtocolItf
{
public:
	Anemoi() = default;
	virtual ~Anemoi() {}
public:
	virtual protocol_nt getProtocolId() const override { return ANEMOI; }
	virtual gen_state_t nextByte(const char c) override;
	virtual bool isNMEA() const override { return false; }

private:
	void incrCRC(const char c) override;
	void parseWind();
	void parseStatus();
};

