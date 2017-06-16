namespace cppback
{
    class BackSingleton
    {
        std::atomic_uint running_ = 0;
        std::future<void> shouldDie_;
        std::promise<void> killBackgroundTasks_;

        friend class BackgroundTaskManager;
    public:
        static BackSingleton& instance()
        {
            static BackSingleton instance;
            return instance;
        }
        BackSingleton(const BackSingleton& rhs) = delete;
        BackSingleton& BackSingleton::operator=(BackSingleton& rhs) = delete;
        BackSingleton(BackSingleton&& rhs) = delete;
        BackSingleton& operator=(BackSingleton&& rhs) = delete;
    protected:
        BackSingleton()
        {
            shouldDie_ = killBackgroundTasks_.get_future();
        }
        ~BackSingleton() = default;
    private:
        template<typename LambdaWithNoArgs>
        void addTask(LambdaWithNoArgs&& func)
        {
            if(isKillSignalSet())
            {
                return;
            }
            cppext::spin([&, func = std::move(func)]() mutable
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

    class BackgroundTaskManager
    {
    public:
        template<typename LambdaWithNoArgs>
        static void addTask(LambdaWithNoArgs&& func)
        {
            BackSingleton::instance().addTask(std::move(func));
        }
        static void kill()
        {
            BackSingleton::instance().kill();
        }
        static bool isKillSignalSet()
        {
            return BackSingleton::instance().isKillSignalSet();
        }

        static bool isKillSignalSet(std::chrono::milliseconds wait)
        {
            return BackSingleton::instance().isKillSignalSet(wait);
        }

        static bool areBackgroundTasksDead(std::chrono::milliseconds wait)
        {
            return BackSingleton::instance().areBackgroundTasksDead(wait);
        }
    };
}