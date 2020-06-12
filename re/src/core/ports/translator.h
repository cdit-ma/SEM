#ifndef BASE_TRANSLATOR_H
#define BASE_TRANSLATOR_H

#include <memory>
#include <exception>
#include <string>

namespace Base{
    template <class BaseType, class MiddlewareType>
    class Translator{
        public:
            static std::unique_ptr<BaseType> MiddlewareToBase(const MiddlewareType& message);
            static std::unique_ptr<MiddlewareType> BaseToMiddleware(const BaseType& message);
    };
    
    class TranslationException : public std::runtime_error{
        public:
            TranslationException(const std::string& what_arg) : std::runtime_error(std::string("Translation Exception: ") + what_arg){};
    };
};

#endif // BASE_TRANSLATOR_H