
/*
	该文件是对TCP传输层协议下的TCP连接对象的封装
 */

#pragma once

#include "BaseTransportConnection.h"
#include "SpinLock.h"

 // TCP传输层客户端(连接对象)
class NET_API TCPTransportConnection : public BaseTransportConnection
{

public:
	TCPTransportConnection();
	~TCPTransportConnection();
	bool Connect(const std::string& IP, uint16_t Port);
#ifdef __linux__
	Task<bool> ConnectAsync(const std::string& IP, uint16_t Port);
#endif
	void Apply(const BaseSocket fd, const sockaddr_in& sockaddr, const SocketType type);
	bool Release();
	bool Send(const Buffer& buffer);

	void BindBufferCallBack(std::function<void(TCPTransportConnection*, Buffer*)> callback);
	void BindRDHUPCallBack(std::function<void(TCPTransportConnection*)> callback);

	SafeQueue<Buffer*>& GetRecvData();
	SafeQueue<Buffer*>& GetSendData();
	CriticalSectionLock& GetSendMtx();

protected:
#ifdef __linux__
	virtual void OnREAD(BaseSocket socket);									// 可读事件
	virtual void OnACCEPT(BaseSocket socket);									// 接受新连接事件
#endif
	virtual void OnREAD(BaseSocket socket, Buffer& buf);						// 可读事件
	virtual void OnACCEPT(BaseSocket socket, BaseSocket newsocket, sockaddr_in addr); // 接受新连接事件
	virtual void OnRDHUP();

protected:
	virtual void OnBindBufferCallBack();
	virtual void OnBindRDHUPCallBack();

private:
	void ProcessRecvQueue();

private:
	SafeQueue<Buffer*> _RecvDatas;
	SafeQueue<Buffer*> _SendDatas;

private:
	std::function<void(TCPTransportConnection*, Buffer*)> _callbackBuffer;
	std::function<void(TCPTransportConnection*)> _callbackRDHUP;
	CriticalSectionLock _SendResMtx;
	SpinLock _ProcessLock;
};


