#ifndef SPIN_H
#define SPIN_H
namespace cppback
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
#endif