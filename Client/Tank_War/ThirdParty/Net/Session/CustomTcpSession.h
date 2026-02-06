#pragma once

#include "EndPoint/PureTCPClient.h"
#include "Session/BaseNetWorkSession.h"
#include "SpinLock.h"

struct CustomPackage
{
	int seq = 0;
	int ack = -1;
	Buffer buffer;
	uint8_t msgType = 0; // 0:null, 1:请求, 2:响应
};

// 基于TCP应用层客户端的自定义通讯协议会话封装
class NET_API CustomTcpSession : public BaseNetWorkSession
{
	// 关注返回值的等待任务
	struct AwaitTask
	{
		int seq = 0;
		Buffer* respsonse;
		std::mutex _mtx;
		std::condition_variable _cv;
		int status = -1; //-1无效，0等待超时，1等待中
		bool time_out = false;
	};

public:
	CustomTcpSession(PureTCPClient* client = nullptr);
	~CustomTcpSession();
	virtual bool Connect(const std::string& IP, uint16_t Port);
#ifdef __linux__
	virtual Task<bool> ConnectAsync(const std::string& IP, uint16_t Port);
#endif
	virtual bool Release();

	bool AsyncSend(const Buffer& buffer);                   // 异步发送，不关心返回结果
	bool AwaitSend(const Buffer& buffer, Buffer& response); // 等待返回结果的发送，关心返回的结果
	PureTCPClient* GetBaseClient();

	void BindRecvRequestCallBack(std::function<void(BaseNetWorkSession*, Buffer* recv, Buffer* resp)> callback);

public:
	virtual bool TryHandshake(uint32_t timeOutMs);
#ifdef __linux__
	virtual Task<bool> TryHandshakeAsync(uint32_t timeOutMs);
#endif
	virtual CheckHandshakeStatus CheckHandshakeTryMsg(Buffer& buffer);
	virtual CheckHandshakeStatus CheckHandshakeConfirmMsg(Buffer& buffer);

protected:
	virtual bool OnSessionClose();
	virtual bool OnRecvData(Buffer* buffer);
	virtual void OnBindRecvDataCallBack();
	virtual void OnBindSessionCloseCallBack();

	void OnBindRecvRequestCallBack();

private:
	bool Send(const Buffer& buffer, int ack = -1); // 异步发送，不关心返回结果
	void ProcessPakage(CustomPackage* newPak = nullptr);
	SpinLock _ProcessLock;

private:
	std::atomic<int> seq;
	SafeQueue<CustomPackage*> _RecvPaks;
	SafeQueue<CustomPackage*> _SendPaks;

	std::function<void(BaseNetWorkSession*, Buffer* recv, Buffer* response)> _callbackRecvRequest;
	SafeMap<int, AwaitTask*> _AwaitMap; // seq->AwaitTask

	Buffer cacheBuffer;
	CustomPackage* cachePak; // 握手/数据包解析缓存

	// 主动握手用
	std::mutex _tryHandshakeMutex;
	std::condition_variable _tryHandshakeCV;
};
