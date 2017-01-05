#pragma once

namespace utils
{
    // For some reason, the compiler I'm on doesn't have c++14 yet, so here is this
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <class T, class U>
    std::unique_ptr<T> acquire_as(std::unique_ptr<U>&& ptr)
    {
        return std::unique_ptr<T>(static_cast<T*>(ptr.release()));
    }
}
