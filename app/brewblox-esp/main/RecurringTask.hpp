#pragma once
#include <asio.hpp>
#include <functional>

class RecurringTask {
public:
    enum class IntervalType {
        FROM_EXECUTION,
        FROM_EXPIRY,
    };
    RecurringTask(asio::io_context& io_,
                  asio::chrono::milliseconds interval_,
                  IntervalType intervalType_,
                  std::function<void()>&& task_)
        : interval(interval_)
        , type(intervalType_)
        , task(task_)
        , timer(io_, interval_)
    {
    }

    ~RecurringTask()
    {
        stop();
    }

    void start()
    {
        timer.expires_from_now(interval);
        run();
    }

    void stop()
    {
        timer.cancel();
    }

private:
    void run()
    {
        timer.async_wait([this](const asio::error_code& e) { onTimeout(e); });
    }

    void onTimeout(const asio::error_code& e)
    {
        if (e) {
            return;
        }

        task();
        if (type == IntervalType::FROM_EXECUTION) {
            timer.expires_from_now(interval);
        } else {
            timer.expires_at(timer.expiry() + interval);
        }
        run();
    }

    asio::chrono::milliseconds interval;
    IntervalType type;
    std::function<void()> task;
    asio::steady_timer timer;
};
