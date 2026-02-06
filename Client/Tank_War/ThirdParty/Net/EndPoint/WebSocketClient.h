#pragma once

#include "TCPEndPoint.h"
#include "SpinLock.h"

enum class WSOpcodeType : uint8_t
{
	WSOpcode_Continue = 0x0,
	WSOpcode_Text = 0x1,
	WSOpcode_Binary = 0x2,
	WSOpcode_Close = 0x8,
	WSOpcode_Ping = 0x9,
	WSOpcode_Pong = 0xA,
};

struct WebSocketPackage
{
	WSOpcodeType opcode = WSOpcodeType::WSOpcode_Binary;
	Buffer buffer;
};

// 基于TCP协议的WebSocket应用层客户端客户端封装
class NET_API WebSocketClient : public TCPEndPoint
{
public:
	WebSocketClient(TCPTransportConnection* con = nullptr);
	WebSocketClient(std::shared_ptr<TCPTransportConnection> con);
	~WebSocketClient();

public:
	virtual bool Connect(const std::string& IP, uint16_t Port);
#ifdef __linux__
	virtual Task<bool> ConnectAsync(const std::string& IP, uint16_t Port);
#endif
	virtual bool Release();

	virtual bool OnRecvBuffer(Buffer* buffer); // 用于绑定网络层(TCP/UDP)触发的Buffer回调
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
	void ProcessPakage(WebSocketPackage* newpak = nullptr);
	SpinLock _ProcessLock;

private:
	SafeQueue<WebSocketPackage*> _RecvPaks;
	SafeQueue<WebSocketPackage*> _SendPaks;

	Buffer cacheBuffer;         // 握手消息/数据帧解析缓冲
	WebSocketPackage* cachePak; // 多帧数据组成的完整帧缓冲

	// 主动握手用
	std::string _SecWsKey;
	std::mutex _tryHandshakeMutex;
	std::condition_variable _tryHandshakeCV;
};