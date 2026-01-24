#pragma once

#include "EndPoint/WebSocketClient.h"
#include "Session/BaseNetWorkSession.h"

struct CustomWebSocketSessionPakage
{
    int seq = 0;
    int ack = -1;
    Buffer buffer;
};

class CustomWebSocketSession : public BaseNetWorkSession
{

    // 关注返回值的等待任务
    struct AwaitTask
    {
        int seq = 0;
        Buffer *respsonse;
        std::mutex _mtx;
        std::condition_variable _cv;
        int status = -1; //-1无效，0等待超时，1等待中
        bool time_out = false;
    };

public:
    CustomWebSocketSession(WebSocketClient *client = nullptr);
    ~CustomWebSocketSession();
    EXPORT_FUNC virtual bool Connect(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual bool Release();

    EXPORT_FUNC virtual bool AsyncSend(const Buffer &buffer);           // 异步发送，不关心返回结果
    EXPORT_FUNC bool AwaitSend(const Buffer &buffer, Buffer &response); // 等待返回结果的发送，关心返回的结果

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
    EXPORT_FUNC bool Send(const Buffer &buffer, int ack = -1);
    EXPORT_FUNC void ProcessPakage(CustomWebSocketSessionPakage *newPak = nullptr);
    SpinLock _ProcessLock;

private:
    SafeQueue<CustomWebSocketSessionPakage *> _RecvPaks;

    std::atomic<int> seq;
    SafeMap<int, AwaitTask *> _AwaitMap; // seq->AwaitTask
};
