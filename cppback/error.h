#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

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