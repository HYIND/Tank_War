
/*
	该文件是对TCP传输层协议下的TCP连接对象的封装
 */

#pragma once

#include "BaseTransportConnection.h"

// TCP传输层客户端(连接对象)
class TCPTransportConnection : public BaseTransportConnection
{

public:
	EXPORT_FUNC TCPTransportConnection();
	EXPORT_FUNC ~TCPTransportConnection();
	EXPORT_FUNC bool Connect(const std::string &IP, uint16_t Port);
	EXPORT_FUNC Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
	EXPORT_FUNC void Apply(const int fd, const sockaddr_in &sockaddr, const SocketType type);
	EXPORT_FUNC bool Release();
	EXPORT_FUNC bool Send(const Buffer &buffer);
	EXPORT_FUNC int Read(Buffer &buffer, int length);

	EXPORT_FUNC void BindBufferCallBack(std::function<void(TCPTransportConnection *, Buffer *)> callback);
	EXPORT_FUNC void BindRDHUPCallBack(std::function<void(TCPTransportConnection *)> callback);

	EXPORT_FUNC SafeQueue<Buffer *> &GetRecvData();
	EXPORT_FUNC SafeQueue<Buffer *> &GetSendData();
	EXPORT_FUNC CriticalSectionLock &GetSendMtx();

protected:
	EXPORT_FUNC virtual void OnRDHUP();
	EXPORT_FUNC virtual void OnREAD(int fd);									// 可读事件
	EXPORT_FUNC virtual void OnREAD(int fd, Buffer &buf);						// 可读事件
	EXPORT_FUNC virtual void OnACCEPT(int fd);									// 接受新连接事件
	EXPORT_FUNC virtual void OnACCEPT(int fd, int newclient, sockaddr_in addr); // 接受新连接事件

protected:
	EXPORT_FUNC virtual void OnBindBufferCallBack();
	EXPORT_FUNC virtual void OnBindRDHUPCallBack();

private:
	void ProcessRecvQueue();

private:
	SafeQueue<Buffer *> _RecvDatas;
	SafeQueue<Buffer *> _SendDatas;

private:
	std::function<void(TCPTransportConnection *, Buffer *)> _callbackBuffer;
	std::function<void(TCPTransportConnection *)> _callbackRDHUP;
	CriticalSectionLock _SendResMtx;
	SpinLock _ProcessLock;
};
