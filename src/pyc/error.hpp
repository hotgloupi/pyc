#pragma once

#include <string>
#include <stdexcept>

namespace pyc { namespace error {

    std::string string(std::exception_ptr e = std::current_exception()) noexcept;

}}
