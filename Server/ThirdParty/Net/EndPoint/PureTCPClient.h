#pragma once

#include "TCPEndPoint.h"
#include "SpinLock.h"

// 基于TCP协议的应用层客户端封装
class NET_API PureTCPClient : public TCPEndPoint
{
public:
	PureTCPClient(TCPTransportConnection* con = nullptr);
	PureTCPClient(std::shared_ptr<TCPTransportConnection> con);
	~PureTCPClient();

public:
	virtual bool Connect(const std::string& IP, uint16_t Port);
#ifdef __linux__
	virtual Task<bool> ConnectAsync(const std::string& IP, uint16_t Port);
#endif
	virtual bool Release();

	virtual bool OnRecvBuffer(Buffer* buffer);
	virtual bool OnConnectClose();

	virtual bool Send(const Buffer& buffer);

public:
	virtual bool TryHandshake(uint32_t timeOutMs);
#ifdef __linux__
	virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs);
#endif
	virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer& buffer);
	virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer& buffer);

protected:
	virtual void OnBindMessageCallBack();
	virtual void OnBindCloseCallBack();

private:
	void ProcessCacheBuffer();

private:
	Buffer cacheBuffer;
	SpinLock _ProcessLock;
};