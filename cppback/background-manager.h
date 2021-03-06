#ifndef BACKGROUND_MANAGER_SINGLETON_H
#define BACKGROUND_MANAGER_SINGLETON_H

#include "cppback/error.h"

#include <future>
#include <type_traits>

namespace cppback
{
    namespace
    {
        template<typename LambdaWithNoArgs>
        auto spin(LambdaWithNoArgs&& func)
        {
            auto task = std::packaged_task<std::result_of_t<LambdaWithNoArgs()>()>{ std::forward<LambdaWithNoArgs>(func) };
            auto res = task.get_future();
            std::thread{ std::move(task) }.detach();
            return res;
        }
    }

    template <class...>
    using void_t = void;

    class AlreadyKilled : public std::runtime_error
    {
    public:
        AlreadyKilled(const std::string msg)
            : runtime_error(msg)
        {};
    };

    class BackgroundManager
    {
        std::atomic_uint running_ = 0;
        std::future<void> shouldDie_;
        std::promise<void> killBackgroundTasks_;
    public:
        BackgroundManager()
        {
            shouldDie_ = killBackgroundTasks_.get_future();
        }

        template<typename LambdaWithNoArgs>
        decltype(auto) addTask(LambdaWithNoArgs&& func)
        {
            return spin([&, func = std::move(func)]() mutable
            {
                if(isKillSignalSet())
                {
                    throw AlreadyKilled("Can't add task after kill signal is already set.");
                }
                return addTaskImpl(std::move(func));
            });
        }

        template<typename LambdaWithNoArgs>
        auto addTaskImpl(LambdaWithNoArgs&& func)
            ->std::enable_if_t
            <
            !(std::is_same_v<std::result_of_t<LambdaWithNoArgs()>, void_t<>>),
            std::result_of_t<LambdaWithNoArgs()>
            >
        {
            std::result_of_t<LambdaWithNoArgs()> result;
            ++running_;
            try
            {
                result = func();
            }
            catch(const std::exception& e)
            {
                --running_;
                throw e;
            }
            --running_;
            return result;
        }

        template<typename LambdaWithNoArgs>
        auto addTaskImpl(LambdaWithNoArgs&& func)
            ->std::enable_if_t
            <
            (std::is_same_v<std::result_of_t<LambdaWithNoArgs()>, void_t<>>),
            void
            >
        {
            ++running_;
            try
            {
                func();
            }
            catch(const std::exception& e)
            {
                --running_;
                throw e;
            }
            --running_;
        }

        void kill()
        {
            if(isKillSignalSet())
            {
                return;
            }
            killBackgroundTasks_.set_value();
        }

        bool isKillSignalSet() const
        {
            using namespace std::literals;
            return isKillSignalSet(0ms);
        }

        bool isKillSignalSet(std::chrono::milliseconds wait) const
        {
            using namespace std::literals;
            return shouldDie_.wait_for(wait) == std::future_status::ready;
        }

        bool isDead(std::chrono::milliseconds wait) const
        {
            if(running_ == 0)
            {
                return true;
            }
            std::this_thread::sleep_for(wait);
            return running_ == 0;
        }

        void sleepInIntervals(std::chrono::milliseconds sleepDuration, std::chrono::milliseconds interval, const std::string& taskName = "") const
        {
            using namespace std::literals;
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            auto durationSlept = 0ms;
            while(durationSlept < sleepDuration)
            {
                if(isKillSignalSet(interval))
                {
                    throw TaskStoppedByKillSignal{ taskName };
                }
                durationSlept = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            }
        }
    };
}
#endif
