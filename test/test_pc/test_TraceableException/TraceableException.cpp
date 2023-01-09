#include <iostream>
#include <exception>
#include <type_traits>

template <typename E>
class TraceableException : public E
{
    static_assert(std::is_base_of<std::exception, E>, "")
public:
    virtual const char* what() const noexcept override
    {
        return "Traceable";
    }
};


int main()
{
    try
    {
        throw std::runtime_error("lflsdjkl");
    }
    catch(...)
    {
        std::nested_exception
    }
    
}