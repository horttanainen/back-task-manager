#ifndef BACKGROUND_MANAGER_H
#define BACKGROUND_MANAGER_H

#include "cppback/background-manager-singleton.h"
#include "cppback/error.h"

#include <chrono>
#include <string>

namespace cppback
{
    class BackgroundManager
    {
    public:
        template<typename LambdaWithNoArgs>
        static void addTask(LambdaWithNoArgs&& func)
        {
            BackgroundManagerSingleton::instance().addTask(std::move(func));
        }
        static void kill()
        {
            BackgroundManagerSingleton::instance().kill();
        }
        static bool isKillSignalSet()
        {
            return BackgroundManagerSingleton::instance().isKillSignalSet();
        }

        static bool isKillSignalSet(std::chrono::milliseconds wait)
        {
            return BackgroundManagerSingleton::instance().isKillSignalSet(wait);
        }

        static bool areBackgroundTasksDead(std::chrono::milliseconds wait)
        {
            return BackgroundManagerSingleton::instance().areBackgroundTasksDead(wait);
        }

        static void sleepInIntervals(std::chrono::milliseconds sleepDuration, std::chrono::milliseconds interval, const std::string& taskName = "")
        {
            using namespace std::literals;
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            auto durationSlept = 0ms;
            while(durationSlept < sleepDuration)
            {
                if(BackgroundManager::isKillSignalSet(interval))
                {
                    throw TaskStoppedByKillSignal{ taskName };
                }
                durationSlept = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            }
        }
    };
}
#endif