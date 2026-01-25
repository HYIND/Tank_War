#pragma once

#include "Connection/TCPTransportConnection.h"
#include "Coroutine.h"
#include "Core/DeleteLater.h"

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
class TCPEndPoint : public DeleteLaterImpl
{
public:
    EXPORT_FUNC TCPEndPoint();
    EXPORT_FUNC virtual ~TCPEndPoint();

    EXPORT_FUNC virtual bool Connect(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual bool Release();

    EXPORT_FUNC virtual bool OnRecvBuffer(Buffer *buffer) = 0;
    EXPORT_FUNC virtual bool OnConnectClose() = 0;

    EXPORT_FUNC virtual bool Send(const Buffer &buffer) = 0;

    EXPORT_FUNC std::shared_ptr<TCPTransportConnection> GetBaseCon();

public:
    // 2表示协议握手所需的字节流长度不足，0表示握手失败，关闭连接，1表示握手成功，建立连接
    EXPORT_FUNC virtual bool TryHandshake(uint32_t timeOutMs) = 0;                         // 作为发起连接的一方，主动发送握手信息
    EXPORT_FUNC virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs) = 0;              // 作为发起连接的一方，主动发送握手信息
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer &buffer) = 0;     // 作为接受连接的一方，检查连接发起者的握手信息，并返回回复信息
    EXPORT_FUNC virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer &buffer) = 0; // 作为发起连接的一方，检查连接接受者的返回的回复信息，若确认则连接建立

    EXPORT_FUNC bool RecvBuffer(TCPTransportConnection *con, Buffer *buffer); // 用于绑定网络层(TCP/UDP)触发的Buffer回调
    EXPORT_FUNC bool ConnectClose(TCPTransportConnection *con);               // 用于绑定网络层(TCP/UDP)触发的RDHUP回调

    EXPORT_FUNC void BindMessageCallBack(std::function<void(TCPEndPoint *, Buffer *)> callback);
    EXPORT_FUNC void BindCloseCallBack(std::function<void(TCPEndPoint *)> callback);

protected:
    EXPORT_FUNC virtual void OnBindMessageCallBack() = 0;
    EXPORT_FUNC virtual void OnBindCloseCallBack() = 0;

protected:
    TCPNetProtocol Protocol;
    std::shared_ptr<TCPTransportConnection> BaseCon;
    bool isHandshakeComplete = false;

    std::function<void(TCPEndPoint *, Buffer *)> _callbackMessage;
    std::function<void(TCPEndPoint *)> _callbackClose;

    CoTimer *_handshaketimeout;
    CriticalSectionLock _Colock;
};
