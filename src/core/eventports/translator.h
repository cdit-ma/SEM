#ifndef BASE_TRANSLATOR_H
#define BASE_TRANSLATOR_H

namespace Base{
    template <class BaseType, class MiddlewareType>
    class Translator{
        public:
            static BaseType* MiddlewareToBase(const MiddlewareType& message);
            static MiddlewareType* BaseToMiddleware(const BaseType& message);
    };
};

#endif // BASE_TRANSLATOR_H