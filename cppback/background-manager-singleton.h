#ifndef BACKGROUND_MANAGER_SINGLETON_H
#define BACKGROUND_MANAGER_SINGLETON_H
namespace cppback
{
    class BackgroundManagerSingleton
    {
        std::atomic_uint running_ = 0;
        std::future<void> shouldDie_;
        std::promise<void> killBackgroundTasks_;

        friend class BackgroundManager;
    public:
        static BackgroundManagerSingleton& instance()
        {
            static BackgroundManagerSingleton instance;
            return instance;
        }
        BackgroundManagerSingleton(const BackgroundManagerSingleton& rhs) = delete;
        BackgroundManagerSingleton& BackgroundManagerSingleton::operator=(BackgroundManagerSingleton& rhs) = delete;
        BackgroundManagerSingleton(BackgroundManagerSingleton&& rhs) = delete;
        BackgroundManagerSingleton& operator=(BackgroundManagerSingleton&& rhs) = delete;
    protected:
        BackgroundManagerSingleton()
        {
            shouldDie_ = killBackgroundTasks_.get_future();
        }
        ~BackgroundManagerSingleton() = default;
    private:
        template<typename LambdaWithNoArgs>
        void addTask(LambdaWithNoArgs&& func)
        {
            if(isKillSignalSet())
            {
                return;
            }
            spin([&, func = std::move(func)]() mutable
            {
                ++running_;
                try
                {
                    func();
                }
                catch(...)
                {
                }
                --running_;
            });
        }

        void kill()
        {
            killBackgroundTasks_.set_value();
        }

        bool isKillSignalSet()
        {
            using namespace std::literals;
            return isKillSignalSet(0ms);
        }

        bool isKillSignalSet(std::chrono::milliseconds wait)
        {
            using namespace std::literals;
            return shouldDie_.wait_for(wait) == std::future_status::ready;
        }

        bool areBackgroundTasksDead(std::chrono::milliseconds wait)
        {
            if(running_ == 0)
            {
                return true;
            }
            std::this_thread::sleep_for(wait);
            return running_ == 0;
        }
    };
}
#endif