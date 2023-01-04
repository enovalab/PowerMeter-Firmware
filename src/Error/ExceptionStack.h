// #ifndef ERROR_EXCEPTIONSTACK_H
// #define ERROR_EXCEPTIONSTACK_H

// #include <exception>
// #include <stdexcept>
// #include <string>
// #include <vector>
// #include <functional>

// namespace Error
// {
//     using ExceptionStack_t = std::vector<std::exception_ptr>;
    
//     class ExceptionStack
//     {
//     public:
//         static ExceptionStack capture();

//         template<typename E>
//         static void catchDeepest(const std::function<void(const E&)>& exceptionHandler);

//         template<typename E>
//         static void catchShallowest(const std::function)

//         std::string what(uint8_t indent = 0, char indentChar = ' ');

//         Iterator begin()

//     private:
//         ExceptionStack_t m_exceptionStack;
//     }
// }

// #endif