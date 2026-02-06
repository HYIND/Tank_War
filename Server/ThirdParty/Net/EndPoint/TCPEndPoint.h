#pragma once

#include "Connection/TCPTransportConnection.h"
#include "Core/DeleteLater.h"

#ifdef __linux__
#include "Coroutine.h"
#endif


enum class TCPNetProtocol
{
	None = 0,
	PureTCP = 10,
	WebSocket = 20
};

enum class CheckHandshakeStatus
{
	None = -1,
	Fail = 0,       // 失败
	Success = 1,    // 成功
	BufferAgain = 2 // 等待数据
};

// TCP终端
class NET_API TCPEndPoint : public DeleteLaterImpl
{
public:
	TCPEndPoint();
	virtual ~TCPEndPoint();

	virtual bool Connect(const std::string& IP, uint16_t Port);
#ifdef __linux__
	virtual Task<bool> ConnectAsync(const std::string& IP, uint16_t Port);
#endif
	virtual bool Release();

	virtual bool OnRecvBuffer(Buffer* buffer) = 0;
	virtual bool OnConnectClose() = 0;

	virtual bool Send(const Buffer& buffer) = 0;

	std::shared_ptr<TCPTransportConnection> GetBaseCon();

public:
	// 2表示协议握手所需的字节流长度不足，0表示握手失败，关闭连接，1表示握手成功，建立连接
	virtual bool TryHandshake(uint32_t timeOutMs) = 0;                         // 作为发起连接的一方，主动发送握手信息
#ifdef __linux__
	virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs) = 0;              // 作为发起连接的一方，主动发送握手信息
#endif
	virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer& buffer) = 0;     // 作为接受连接的一方，检查连接发起者的握手信息，并返回回复信息
	virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer& buffer) = 0; // 作为发起连接的一方，检查连接接受者的返回的回复信息，若确认则连接建立

	bool RecvBuffer(TCPTransportConnection* con, Buffer* buffer); // 用于绑定网络层(TCP/UDP)触发的Buffer回调
	bool ConnectClose(TCPTransportConnection* con);               // 用于绑定网络层(TCP/UDP)触发的RDHUP回调

	void BindMessageCallBack(std::function<void(TCPEndPoint*, Buffer*)> callback);
	void BindCloseCallBack(std::function<void(TCPEndPoint*)> callback);

protected:
	virtual void OnBindMessageCallBack() = 0;
	virtual void OnBindCloseCallBack() = 0;

protected:
	TCPNetProtocol Protocol;
	std::shared_ptr<TCPTransportConnection> BaseCon;
	bool isHandshakeComplete = false;

	std::function<void(TCPEndPoint*, Buffer*)> _callbackMessage;
	std::function<void(TCPEndPoint*)> _callbackClose;

#ifdef __linux__
	CoTimer* _handshaketimeout;
	CriticalSectionLock _Colock;
#endif
};
