#ifndef LOOP_SIGNAL_CHECKER_H
#define LOOP_SIGNAL_CHECKER_H
namespace cppback
{
    class LoopSignalChecker
    {
        int counter_ = 0;
        const int interval_;
        const std::string task_;
    public:
        LoopSignalChecker(int intervalToCheck, std::string task = "")
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