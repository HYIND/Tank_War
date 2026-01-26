#pragma once

#include <memory>
#include <coroutine>
#include <optional>
#include <type_traits>
#include <utility>
#include "CriticalSectionLock.h"
#include <netinet/in.h>
#include <atomic>
#include <stdexcept>
#include <thread>

#include "PublicShareExportMacro.h"

struct PUBLICSHARE_API RegisterTaskAwaiter
{
    bool await_ready();
    void await_suspend(std::coroutine_handle<> coro);
    void await_resume();
};

struct PUBLICSHARE_API TaskHandle
{
    std::coroutine_handle<> coroutine;

    TaskHandle(std::coroutine_handle<> coroutine);
    ~TaskHandle();
};

template <typename T>
class Task;

template <typename T>
struct TaskPromiseBase
{
    std::exception_ptr exception_;
    std::coroutine_handle<> continuation_;

    std::shared_ptr<CriticalSectionLock> _continuationlock = std::make_shared<CriticalSectionLock>();
    std::shared_ptr<ConditionVariable> _syncwaitcv = std::make_shared<ConditionVariable>();

    auto initial_suspend() noexcept
    {
        return RegisterTaskAwaiter{};
    }

    auto final_suspend() noexcept
    {
        struct FinalAwaiter
        {
            TaskPromiseBase *promise;
            bool await_ready() noexcept { return false; }
            std::coroutine_handle<> await_suspend(std::coroutine_handle<> completed_coro) noexcept
            {
                LockGuard lock(*(promise->_continuationlock));
                auto continuation = promise->continuation_;
                if (continuation)
                {
                    promise->_syncwaitcv->NotifyAll();
                    return continuation;
                }
                promise->_syncwaitcv->NotifyAll();
                return std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };
        return FinalAwaiter{this};
    }

    void unhandled_exception() noexcept
    {
        exception_ = std::current_exception();
    }

    void set_continuation(std::coroutine_handle<> continuation) noexcept
    {
        continuation_ = continuation;
    }
};

// 非 void 类型的 Promise
template <typename T>
struct TaskPromise : TaskPromiseBase<T>
{
    std::optional<T> value_;

    Task<T> get_return_object() noexcept;

    void return_value(T &&value)
    {
        value_.emplace(std::move(value));
    }

    void return_value(const T &value)
    {
        value_.emplace(value);
    }

    T &result()
    {
        if (this->exception_)
        {
            std::rethrow_exception(this->exception_);
        }
        return value_.value();
    }

    ~TaskPromise() = default;
};

// void 特化的 Promise
template <>
struct TaskPromise<void> : TaskPromiseBase<void>
{
    Task<void> get_return_object() noexcept;

    void return_void() noexcept {}

    void result()
    {
        if (exception_)
        {
            std::rethrow_exception(exception_);
        }
    }

    ~TaskPromise() = default;
};

template <typename T>
struct TaskAwaiter
{
    std::coroutine_handle<TaskPromise<T>> coroutine_;
    std::shared_ptr<CriticalSectionLock> _continuationlock;
    TaskAwaiter(std::coroutine_handle<TaskPromise<T>> coroutine, std::shared_ptr<CriticalSectionLock> lock) noexcept
        : coroutine_(coroutine), _continuationlock(lock) {}

    bool await_ready() noexcept
    {
        return coroutine_ && coroutine_.done();
    }

    bool await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept
    {
        if (!coroutine_ || coroutine_.done())
        {
            return false; // 无效协程，不挂起
        }

        LockGuard lock(*_continuationlock);
        if (coroutine_.done())
            return false;

        auto &promise = coroutine_.promise();
        promise.set_continuation(awaiting_coroutine);

        return true;
    }

    // 非 void 版本的 await_resume
    template <typename U = T>
    std::enable_if_t<!std::is_void_v<U>, U> await_resume()
    {
        if (!coroutine_ || coroutine_.done())
        {
            if (coroutine_)
            {
                return coroutine_.promise().result();
            }
            throw std::runtime_error("Invalid coroutine");
        }
        throw std::runtime_error("Coroutine not completed");
    }

    // void 特化的 await_resume
    template <typename U = T>
    std::enable_if_t<std::is_void_v<U>, void> await_resume()
    {
        if (!coroutine_ || coroutine_.done())
        {
            if (coroutine_)
            {
                coroutine_.promise().result();
                return;
            }
            throw std::runtime_error("Invalid coroutine");
        }
        throw std::runtime_error("Coroutine not completed");
    }
};

template <typename T>
class Task
{
public:
    using promise_type = TaskPromise<T>;
    using value_type = T;

    explicit Task(std::coroutine_handle<promise_type> coroutine, std::shared_ptr<CriticalSectionLock> lock, std::shared_ptr<ConditionVariable> cv) noexcept
        : coroutine_(coroutine), _continuationlock(lock), _syncwaitcv(cv) {}

    Task(Task &&other) noexcept
        : coroutine_(std::exchange(other.coroutine_, nullptr)), _continuationlock(other._continuationlock) {}

    Task &operator=(Task &&other) noexcept
    {
        if (this != &other)
        {
            coroutine_ = std::exchange(other.coroutine_, nullptr);
            _continuationlock = other._continuationlock;
        }
        return *this;
    }

    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;

    ~Task()
    {
    }

    TaskAwaiter<T> operator co_await() const noexcept
    {
        return TaskAwaiter<T>{coroutine_, _continuationlock};
    }

    template <typename U = T>
    std::enable_if_t<!std::is_void_v<U>, U> sync_wait()
    {
        if (coroutine_)
        {
            LockGuard guard(*_continuationlock);
            if (coroutine_.done())
            {
                return coroutine_.promise().result();
            }
            _syncwaitcv->Wait(guard);
            return coroutine_.promise().result();
        }
        throw std::runtime_error("Invalid task");
    }

    template <typename U = T>
    std::enable_if_t<std::is_void_v<U>, void> sync_wait()
    {
        if (coroutine_)
        {
            LockGuard guard(*_continuationlock);
            if (coroutine_.done())
            {
                coroutine_.promise().result();
                return;
            }
            _syncwaitcv->Wait(guard);
            coroutine_.promise().result();
            return;
        }
        throw std::runtime_error("Invalid task");
    }

    bool is_done() const noexcept
    {
        return !coroutine_ || coroutine_.done();
    }

    auto get_handle() const noexcept
    {
        return coroutine_;
    }

private:
    std::coroutine_handle<promise_type> coroutine_;
    std::shared_ptr<CriticalSectionLock> _continuationlock;
    std::shared_ptr<ConditionVariable> _syncwaitcv;

    template <typename U>
    friend struct TaskPromise;
};

template <typename T>
inline Task<T> TaskPromise<T>::get_return_object() noexcept
{
    return Task<T>(
        std::coroutine_handle<TaskPromise<T>>::from_promise(*this),
        TaskPromiseBase<T>::_continuationlock,
        TaskPromiseBase<T>::_syncwaitcv);
}

inline Task<void> TaskPromise<void>::get_return_object() noexcept
{
    return Task<void>(
        std::coroutine_handle<TaskPromise<void>>::from_promise(*this),
        TaskPromiseBase<void>::_continuationlock,
        TaskPromiseBase<void>::_syncwaitcv);
}

namespace CoroTask
{
    template <typename U>
    struct is_task : std::false_type
    {
    };
    template <typename T>
    struct is_task<Task<T>> : std::true_type
    {
    };
    template <typename T>
    static constexpr bool is_task_v = is_task<T>::value;

    template <typename T>
    struct flatten_task
    {
        using type = T;
    };

    template <typename T>
    struct flatten_task<Task<T>>
    {
        using type = T; // 展开一层
    };

    template <typename T>
    using flatten_task_t = typename flatten_task<T>::type;

    template <typename Callable>
    static auto Run(Callable callable)
        -> Task<flatten_task_t<decltype(callable())>>
    {
        using RawReturnType = decltype(callable());

        if constexpr (is_task_v<RawReturnType>)
        {
            co_return co_await callable();
        }
        else if constexpr (std::is_void_v<RawReturnType>)
        {
            callable();
            co_return;
        }
        else
        {
            co_return callable();
        }
    }
}

// 协程定时器包装器
class PUBLICSHARE_API CoTimer
{
public:
    enum class WakeType
    {
        Error = -1,
        RUNNING = 0,
        TIMEOUT,
        MANUAL_WAKE
    };

    struct Handle
    {
        Handle();
        ~Handle();

        int fd;
        bool active;

        std::coroutine_handle<> coroutine;
        std::atomic<bool> corodone;
        CriticalSectionLock corolock;

        std::atomic<WakeType> result;
    };

    struct Awaiter
    {
        Awaiter(std::shared_ptr<CoTimer::Handle> handle);

        bool await_ready() const noexcept;
        void await_suspend(std::coroutine_handle<> coro);
        WakeType await_resume() noexcept;

        std::shared_ptr<CoTimer::Handle> handle;
        std::coroutine_handle<> coroutine;
    };

public:
    CoTimer(std::chrono::milliseconds timeout);
    ~CoTimer();
    Awaiter operator co_await(); // 协程等待操作
    void wake();                 // 立即唤醒

private:
    std::shared_ptr<Handle> handle;
};

Task<bool> PUBLICSHARE_API CoSleep(std::chrono::milliseconds timeout);

// 协程连接器包装器
class PUBLICSHARE_API CoConnection
{
public:
    struct Handle
    {
        Handle();
        ~Handle();

        int fd;
        sockaddr_in addr;

        bool active;

        std::coroutine_handle<> coroutine;
        std::atomic<bool> corodone;
        CriticalSectionLock corolock;

        int res;
    };

    struct Awaiter
    {
        Awaiter(std::shared_ptr<Handle> handle);

        bool await_ready();
        void await_suspend(std::coroutine_handle<> coro);
        int await_resume();

        std::shared_ptr<Handle> handle;
        std::coroutine_handle<> coroutine;
    };

public:
    CoConnection(int fd, sockaddr_in addr);
    ~CoConnection();
    Awaiter operator co_await();

private:
    std::shared_ptr<Handle> handle;
};
