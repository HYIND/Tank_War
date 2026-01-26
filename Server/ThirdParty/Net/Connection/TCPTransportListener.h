
/*
	该文件是对TCP传输层协议下的TCP连接对象的封装
 */

#pragma once

#include "TCPTransportConnection.h"

 // TCP传输层监听器
class NET_API TCPTransportListener : public BaseTransportConnection
{

public:
	TCPTransportListener();
	~TCPTransportListener();
	bool Listen(const std::string& IP, int Port);
	bool ReleaseListener();
	bool ReleaseClients();
	void BindAcceptCallBack(std::function<void(std::shared_ptr<TCPTransportConnection>)> callback);

protected:
#ifdef __linux__
	virtual void OnREAD(BaseSocket socket);									// 可读事件
	virtual void OnACCEPT(BaseSocket socket);									// 接受新连接事件
#endif
	virtual void OnREAD(BaseSocket socket, Buffer& buf);						// 可读事件
	virtual void OnACCEPT(BaseSocket socket, BaseSocket newsocket, sockaddr_in addr); // 接受新连接事件
	virtual void OnRDHUP();

private:
	std::function<void(std::shared_ptr<TCPTransportConnection>)> _callbackAccept;
};
