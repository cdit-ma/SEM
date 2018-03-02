#ifndef BASE_TRANSLATER_H
#define BASE_TRANSLATER_H

namespace Base{

    template <class BaseType, class MiddlewareType>
    class Translater{
        public:
            BaseType* MiddlewareToBase(const MiddlewareType& message);
            MiddlewareType* BaseToMiddleware(const BaseType& message);
    };
};

#endif // BASE_TRANSLATE_H