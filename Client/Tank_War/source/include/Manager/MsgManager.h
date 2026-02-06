#pragma once

#include "stdafx.h"
#include "Net/Helper/Buffer.h"

class MsgManager
{
public:
	static MsgManager* Instance();

public:
	void ProcessMsg(const json& js);

private:
	MsgManager();
	~MsgManager();
};

#define MSGMANAGER MsgManager::Instance()