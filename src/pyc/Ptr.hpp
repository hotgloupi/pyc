#pragma once

#include <memory>

namespace pyc {

    template<typename T>
    using Ptr = std::unique_ptr<T>;


    template<typename T, typename... Args>
    Ptr<T> make_unique(Args&&... args)
    {
        return Ptr<T>(new T(std::forward<Args>(args)...));
    }

}
