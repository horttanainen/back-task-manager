#ifndef ERROR_H
#define ERROR_H

namespace cppback
{
    class TaskStoppedByKillSignal : public std::runtime_error
    {
    public:
        TaskStoppedByKillSignal(const std::string& msg) : runtime_error(msg)
        {};
    };
}
#endif