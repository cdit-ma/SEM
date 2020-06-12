#ifndef PRIMITIVE_TRANSLATOR_H
#define PRIMITIVE_TRANSLATOR_H

#include <stdexcept>
#include <typeinfo>
#include <iostream>

#include <limits>

// REVIEW (Mitch): There must be a better way of doing these conversions and checks.
//  See test file in tests/middleware/base/primitivetranslations/tests.cpp for usage pattern.
//  Generally used to convert TAO types....
template <class FromType, class ToType>
ToType TranslatePrimitive(const FromType& value){
    ToType translated_type = value;

    if(std::is_signed<FromType>::value && std::is_unsigned<ToType>::value){
        if(value < 0){
            throw std::invalid_argument("Can't convert from negative to Unsigned");
        }
    }

    if(std::is_unsigned<FromType>::value && value > std::numeric_limits<ToType>::max()){
        throw std::invalid_argument("Can't convert from large unsigned to other type");
    }
    
   if(translated_type != value){
        std::string except = "Can't convert from type: '";
        except += std::string(typeid(FromType).name());
        except += "' TO '";
        except += std::string(typeid(ToType).name());
        except += "'";
        throw std::invalid_argument(except);
    }else{
        return translated_type;
    }
};


#endif //PRIMITIVE_TRANSLATOR_H
