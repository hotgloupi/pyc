#include <pyc/error.hpp>

namespace pyc { namespace error {

    std::string string(std::exception_ptr e) noexcept
    {
        try
        {
            std::rethrow_exception(e);
        }
        catch (std::exception const& err)
        {
            return err.what();
        }
        catch (...)
        {
            return "Unknown exception type";
        }
    }
}}
