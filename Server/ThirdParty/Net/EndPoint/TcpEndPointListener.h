#pragma once

#include "EndPoint/TCPEndPoint.h"
#include "Coroutine.h"
#include "Timer.h"

struct ClientData;

// 用于监听指定协议的TCP连接，用于校验连接协议
class TcpEndPointListener
{
public:
    EXPORT_FUNC TcpEndPointListener(TCPNetProtocol proto = TCPNetProtocol::PureTCP);
    EXPORT_FUNC ~TcpEndPointListener();

    EXPORT_FUNC TCPNetProtocol Protocol();
    EXPORT_FUNC void SetProtocol(const TCPNetProtocol &proto);
    EXPORT_FUNC bool Listen(const std::string &IP, int Port);
    EXPORT_FUNC void BindEstablishConnectionCallBack(std::function<void(TCPEndPoint *)> callback);

private:
    EXPORT_FUNC void RecvCon(std::shared_ptr<TCPTransportConnection> waitCon);
    EXPORT_FUNC void ConClose(TCPTransportConnection *Con);
    EXPORT_FUNC void Handshake(TCPTransportConnection *waitCon, Buffer *buf);
    EXPORT_FUNC void CleanExpiredClient();

private:
    std::shared_ptr<TCPTransportListener> BaseListener;
    TCPNetProtocol _Protocol;
    std::function<void(TCPEndPoint *)> _callBackEstablish;
    SafeArray<std::shared_ptr<ClientData>> waitClients; // 等待校验协议的客户端
    std::shared_ptr<TimerTask> CleanExpiredTask;
};
