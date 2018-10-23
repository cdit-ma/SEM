#ifndef BASE_TRANSLATOR_H
#define BASE_TRANSLATOR_H

#include <memory>
namespace Base{
    template <class BaseType, class MiddlewareType>
    class Translator{
        public:
            static std::unique_ptr<BaseType> MiddlewareToBase(const MiddlewareType& message);
            static std::unique_ptr<MiddlewareType> BaseToMiddleware(const BaseType& message);
    };
};

#endif // BASE_TRANSLATOR_H