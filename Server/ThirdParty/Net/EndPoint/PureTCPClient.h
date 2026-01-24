#pragma once

#include "TCPEndPoint.h"
#include "SpinLock.h"

// 基于TCP协议的应用层客户端封装
class PureTCPClient : public TCPEndPoint
{
public:
    EXPORT_FUNC PureTCPClient(TCPTransportConnection *con = nullptr);
    EXPORT_FUNC PureTCPClient(std::shared_ptr<TCPTransportConnection> con);
    EXPORT_FUNC ~PureTCPClient();

public:
    EXPORT_FUNC virtual bool Connect(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual bool Release();

    EXPORT_FUNC virtual bool OnRecvBuffer(Buffer *buffer);
    EXPORT_FUNC virtual bool OnConnectClose();

    EXPORT_FUNC virtual bool Send(const Buffer &buffer);

public:
    EXPORT_FUNC virtual bool TryHandshake(uint32_t timeOutMs);
    EXPORT_FUNC virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs);
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer &buffer);
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer &buffer);

protected:
    EXPORT_FUNC virtual void OnBindMessageCallBack();
    EXPORT_FUNC virtual void OnBindCloseCallBack();

private:
    EXPORT_FUNC void ProcessCacheBuffer();

private:
    Buffer cacheBuffer;
    SpinLock _ProcessLock;
};