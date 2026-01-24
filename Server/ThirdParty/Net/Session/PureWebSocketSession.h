#pragma once

#include "EndPoint/WebSocketClient.h"
#include "Session/BaseNetWorkSession.h"

struct PureWebSocketSessionPakage
{
    Buffer buffer;
};

class PureWebSocketSession : public BaseNetWorkSession
{

public:
    PureWebSocketSession(WebSocketClient *client = nullptr);
    ~PureWebSocketSession();
    EXPORT_FUNC virtual bool Connect(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual bool Release();

    EXPORT_FUNC virtual bool AsyncSend(const Buffer &buffer); // 异步发送，不关心返回结果

    EXPORT_FUNC WebSocketClient *GetBaseClient();

public:
    EXPORT_FUNC virtual bool TryHandshake(uint32_t timeOutMs);
    EXPORT_FUNC virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs);
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer &buffer);
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer &buffer);

protected:
    EXPORT_FUNC virtual bool OnSessionClose();
    EXPORT_FUNC virtual bool OnRecvData(Buffer *buffer);
    EXPORT_FUNC virtual void OnBindRecvDataCallBack();
    EXPORT_FUNC virtual void OnBindSessionCloseCallBack();

private:
    EXPORT_FUNC bool Send(const Buffer &buffer);
    EXPORT_FUNC void ProcessPakage(PureWebSocketSessionPakage *newPak = nullptr);
    SpinLock _ProcessLock;

private:
    SafeQueue<PureWebSocketSessionPakage *> _RecvPaks;
};
