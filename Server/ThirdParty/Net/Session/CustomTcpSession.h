#pragma once

#include "EndPoint/PureTCPClient.h"
#include "Session/BaseNetWorkSession.h"
#include "SpinLock.h"
#include "Coroutine.h"

struct CustomPackage
{
    int seq = 0;
    int ack = -1;
    Buffer buffer;
    uint8_t msgType = 0; // 0:null, 1:请求, 2:响应
};

// 基于TCP应用层客户端的自定义通讯协议会话封装
class CustomTcpSession : public BaseNetWorkSession
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
    CustomTcpSession(PureTCPClient *client = nullptr);
    ~CustomTcpSession();
    EXPORT_FUNC virtual bool Connect(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual Task<bool> ConnectAsync(const std::string &IP, uint16_t Port);
    EXPORT_FUNC virtual bool Release();

    EXPORT_FUNC bool AsyncSend(const Buffer &buffer);                   // 异步发送，不关心返回结果
    EXPORT_FUNC bool AwaitSend(const Buffer &buffer, Buffer &response); // 等待返回结果的发送，关心返回的结果
    EXPORT_FUNC PureTCPClient *GetBaseClient();

    EXPORT_FUNC void BindRecvRequestCallBack(std::function<void(BaseNetWorkSession *, Buffer *recv, Buffer *resp)> callback);

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

    EXPORT_FUNC void OnBindRecvRequestCallBack();

private:
    EXPORT_FUNC bool Send(const Buffer &buffer, int ack = -1); // 异步发送，不关心返回结果
    EXPORT_FUNC void ProcessPakage(CustomPackage *newPak = nullptr);
    SpinLock _ProcessLock;

private:
    std::atomic<int> seq;
    SafeQueue<CustomPackage *> _RecvPaks;
    SafeQueue<CustomPackage *> _SendPaks;

    std::function<void(BaseNetWorkSession *, Buffer *recv, Buffer *response)> _callbackRecvRequest;
    SafeMap<int, AwaitTask *> _AwaitMap; // seq->AwaitTask

    Buffer cacheBuffer;
    CustomPackage *cachePak; // 握手/数据包解析缓存

    // 主动握手用
    std::mutex _tryHandshakeMutex;
    std::condition_variable _tryHandshakeCV;
};
