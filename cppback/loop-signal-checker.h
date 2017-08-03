#ifndef LOOP_SIGNAL_CHECKER_H
#define LOOP_SIGNAL_CHECKER_H

#include "cppback/background-manager.h"

#include <string>

namespace cppback
{
    class LoopSignalChecker
    {
        size_t counter_ = 0;
        const size_t interval_;
        const std::string task_;
    public:
        LoopSignalChecker(size_t intervalToCheck, std::string task = "")
            :interval_{ intervalToCheck }
            , task_{ task }
        {}

        void check()
        {
            if(counter_ % interval_ == 0)
            {
                if(BackgroundManager::isKillSignalSet())
                {
                    throw TaskStoppedByKillSignal{ task_ };
                }
            }
            ++counter_;
        }
    };
}
#endif