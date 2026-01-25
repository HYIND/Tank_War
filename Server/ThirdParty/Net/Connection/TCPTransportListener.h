
/*
	该文件是对TCP传输层协议下的TCP连接对象的封装
 */

#pragma once

#include "TCPTransportConnection.h"

// TCP传输层监听器
class TCPTransportListener : public BaseTransportConnection
{

public:
	EXPORT_FUNC TCPTransportListener();
	EXPORT_FUNC ~TCPTransportListener();
	EXPORT_FUNC bool Listen(const std::string &IP, int Port);
	EXPORT_FUNC bool ReleaseListener();
	EXPORT_FUNC bool ReleaseClients();
	EXPORT_FUNC void BindAcceptCallBack(std::function<void(std::shared_ptr<TCPTransportConnection>)> callback);

protected:
	EXPORT_FUNC virtual void OnRDHUP();
	EXPORT_FUNC virtual void OnREAD(int fd);									// 可读事件
	EXPORT_FUNC virtual void OnREAD(int fd, Buffer &buf);						// 可读事件
	EXPORT_FUNC virtual void OnACCEPT(int fd);									// 接受新连接事件
	EXPORT_FUNC virtual void OnACCEPT(int fd, int newclient, sockaddr_in addr); // 接受新连接事件

private:
	std::function<void(std::shared_ptr<TCPTransportConnection>)> _callbackAccept;
};
