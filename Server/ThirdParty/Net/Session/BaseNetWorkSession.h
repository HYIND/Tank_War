#pragma once

#include "EndPoint/TCPEndPoint.h"
#include "Core/DeleteLater.h"
#include "Coroutine.h"
#include "CriticalSectionLock.h"
class BaseNetWorkSession : public DeleteLaterImpl
{

public:
    BaseNetWorkSession();
    virtual ~BaseNetWorkSession();
    virtual bool Connect(const std::string &IP, uint16_t Port);
    virtual Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
    virtual bool Release();

public: // 供Listener/EndPoint调用,须继承实现
    EXPORT_FUNC virtual bool AsyncSend(const Buffer &buffer) = 0;
    EXPORT_FUNC virtual bool TryHandshake(uint32_t timeOutMs) = 0;
    EXPORT_FUNC virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs) = 0;
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer &buffer) = 0;
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer &buffer) = 0;

public: // 供外部调用
    EXPORT_FUNC void BindRecvDataCallBack(std::function<void(BaseNetWorkSession *, Buffer *recv)> callback);
    EXPORT_FUNC void BindSessionCloseCallBack(std::function<void(BaseNetWorkSession *)> callback);
    EXPORT_FUNC char *GetIPAddr();
    EXPORT_FUNC uint16_t GetPort();

public: // 供Listener/EndPoint调用
    EXPORT_FUNC void RecvData(TCPEndPoint *client, Buffer *buffer);
    EXPORT_FUNC void SessionClose(TCPEndPoint *client);
    EXPORT_FUNC TCPEndPoint *GetBaseClient();

protected: // 须继承实现
    EXPORT_FUNC virtual bool OnSessionClose() = 0;
    EXPORT_FUNC virtual bool OnRecvData(Buffer *buffer) = 0;
    EXPORT_FUNC virtual void OnBindRecvDataCallBack() = 0;
    EXPORT_FUNC virtual void OnBindSessionCloseCallBack() = 0;

protected:
    TCPEndPoint *BaseClient;

    bool isHandshakeComplete;

    std::function<void(BaseNetWorkSession *, Buffer *recv)> _callbackRecvData;
    std::function<void(BaseNetWorkSession *)> _callbackSessionClose;

    CoTimer *_handshaketimeout;
    CriticalSectionLock _Colock;
};
