/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "SetupNG.h"
#include "MenuEntry.h"



class SetupAction final : public MenuEntry
{
public:
	explicit SetupAction(const char *title, int (*action)(SetupAction *), int code, bool _end_menu=false);
	virtual ~SetupAction() = default;
	void enter() override;
	void exit(int ups=1);
	void display( int mode=0 );
	const char* value() const override { return ""; };
	int getCode() const { return _code; }
	// Rotoary API
	void up(int count) override {};
	void down(int count) override {};
	void press() override;
	void release() override {};
	void longPress() override;

private:
	int (*_action)( SetupAction *p );
	int _code;
	bool _end_menu;
	int  _extra_ups = 0;
};
