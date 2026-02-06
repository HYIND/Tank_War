#pragma once

#include "Coroutine.h"
#include <liburing.h>
#include "ThreadPool.h"

struct Coro_IOuringOPData;

class CoroutineScheduler
{
public:
    static CoroutineScheduler *Instance();
    int Run();
    void Stop();
    bool Running();

public: // Timer
    std::shared_ptr<CoTimer::Handle> create_timer(std::chrono::milliseconds interval);
    void wake_timer(std::shared_ptr<CoTimer::Handle> weakhandle);

public: // Task
    std::shared_ptr<TaskHandle> RegisterTaskCoroutine(std::coroutine_handle<> coroutine);

public: // Connect
    std::shared_ptr<CoConnection::Handle> create_connection(int fd, sockaddr_in addr);

private:
    CoroutineScheduler();
    void LoopSubmitIOEvent();
    void Loop();
    bool GetDoneIOEvents(std::vector<Coro_IOuringOPData *> &opdatas);
    int EventProcess(Coro_IOuringOPData *opdata);
    bool AddReadShutDownEvent(Coro_IOuringOPData *opdata);
    void DoPostIOEvents(std::vector<Coro_IOuringOPData *> &opdatas);

private:
    bool SubmitTimerEvent(Coro_IOuringOPData *opdata);
    bool SubmitCoroutineEvent(Coro_IOuringOPData *opdata);
    bool SubmitConnectEvent(Coro_IOuringOPData *opdata);

    template <typename Callable>
    void ExcuteCoroutine(Callable &&callable)
    {
        _ExcuteEventProcessPool.submit([callable = std::forward<Callable>(callable)]() mutable
                                       {
            try
            {
                std::atomic_thread_fence(std::memory_order_acquire);
                callable();
                std::atomic_thread_fence(std::memory_order_release);
            }
            catch (const std::exception &e)
            {
                std::cerr << "ExcuteCoroutine task Error: " << e.what() << '\n';
            } });
    }

private:
    bool _shouldshutdown;
    bool _isrunning;
    bool _isinitsuccess;
    io_uring ring;

    SafeQueue<Coro_IOuringOPData *> _optaskqueue;

    ThreadPool _ExcuteEventProcessPool;

    std::mutex _IOEventLock;
    std::condition_variable _IOEventCV;

    CriticalSectionLock _doPostIOEventLock;
};