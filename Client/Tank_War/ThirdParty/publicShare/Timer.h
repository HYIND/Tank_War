#pragma once

#include <functional>
#include <memory>
#include <string>

#include "PublicShareExportMacro.h"

struct TimerTaskHandle;

class PUBLICSHARE_API TimerTask
{
public:
    using Callback = std::function<void()>;

    static std::shared_ptr<TimerTask> CreateOnce(
        const std::string &name,
        uint64_t delay_ms,
        Callback callback);

    static std::shared_ptr<TimerTask> CreateRepeat(
        const std::string &name,
        uint64_t interval_ms,
        Callback callback,
        uint64_t delay_ms = 0);

    bool Run();
    bool Stop();

    ~TimerTask();
    void Clean(); // 关闭定时器并清理资源

public:
    const std::string &name();
    uint64_t interval();
    bool is_repeat();
    bool is_valid();
    int timer_fd();
    Callback callback();
    void mark_invalid();

private:
    TimerTask(const std::string &name,
              uint64_t interval_ms,
              bool repeat,
              Callback callback,
              uint64_t delay_ms = 0);

    TimerTask(const TimerTask &) = delete;
    TimerTask &operator=(const TimerTask &) = delete;

    bool create_timer_fd();

private:
    std::shared_ptr<TimerTaskHandle> _handle;
};
