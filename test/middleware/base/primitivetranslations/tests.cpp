
#include <core/ports/primitivetranslator.hpp>
#include <limits>
#include <gtest/gtest.h>

typedef int RE_integer;
typedef float RE_float;
typedef double RE_double;
typedef char RE_char;

template<class T, class S>
bool IsValidConversion(const T& value){
    bool success = false;
    try{
        S converted = TranslatePrimitive<T, S>(value);
        success = true;
    }catch(const std::invalid_argument& e){
    }
    return success;
}

TEST(re2primitive, integer2short){
    typedef short CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_FALSE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_FALSE(min_test);
    }
};

TEST(re2primitive, integer2ushort){
    typedef unsigned short CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_FALSE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_FALSE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_FALSE(min_test);
    }
};

TEST(re2primitive, integer2integer){
    typedef int CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(re2primitive, integer2uinteger){
    typedef unsigned int CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_FALSE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_FALSE(min_test);
    }
};

TEST(re2primitive, integer2long){
    typedef long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(re2primitive, integer2ulong){
    typedef unsigned long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_FALSE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_FALSE(min_test);
    }
};

TEST(re2primitive, integer2longlong){
    typedef long long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(re2primitive, integer2ulonglong){
    typedef unsigned long long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    
    {
        RE_TYPE small = 16;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_FALSE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_FALSE(min_test);
    }
};

TEST(re2primitive, double2double){
    typedef double CORBA_TYPE;
    typedef RE_double RE_TYPE;
    
    {
        RE_TYPE small = 16.0;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(re2primitive, float2float){
    typedef float CORBA_TYPE;
    typedef RE_float RE_TYPE;
    
    {
        RE_TYPE small = 16.0f;
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE neg_small = -16.0f;

        auto neg_small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(re2primitive, char2char){
    typedef char CORBA_TYPE;
    typedef RE_char RE_TYPE;
    
    {
        RE_TYPE small = 'a';
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(re2primitive, char2wchar){
    typedef wchar_t CORBA_TYPE;
    typedef RE_char RE_TYPE;
    
    {
        RE_TYPE small = 'a';
        
        auto small_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        RE_TYPE max = std::numeric_limits<RE_TYPE>::max();
        
        auto max_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        RE_TYPE min = std::numeric_limits<RE_TYPE>::min();
        
        auto min_test = IsValidConversion<RE_TYPE, CORBA_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};


TEST(corba2re, short2integer){
    typedef short CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        CORBA_TYPE neg_small = -16;

        auto neg_small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(corba2re, ushort2integer){
    typedef unsigned short CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        EXPECT_TRUE(max_test);
    }

    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(corba2re, uinteger2integer){
    typedef unsigned int CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(max_test);
    }

    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(corba2re, integer2integer){
    typedef int CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        EXPECT_TRUE(max_test);
    }
    
    {
        CORBA_TYPE neg_small = -16;

        auto neg_small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(corba2re, long2integer){
    typedef long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(max_test);
    }
    
    {
        CORBA_TYPE neg_small = -16;

        auto neg_small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(min_test);
    }
};

TEST(corba2re, ulong2integer){
    typedef unsigned long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE signed_max = std::numeric_limits<long>::max();
        
        auto signed_max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(signed_max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(signed_max_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(max_test);
    }
    
    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};


TEST(corba2re, longlong2integer){
    typedef long long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(max_test);
    }
    
    {
        CORBA_TYPE neg_small = -16;

        auto neg_small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(neg_small);
        EXPECT_TRUE(neg_small_test);
    }

    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(min_test);
    }
};

TEST(corba2re, ulonglong2integer){
    typedef unsigned long long CORBA_TYPE;
    typedef RE_integer RE_TYPE;
    {
        CORBA_TYPE small = 16;
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE signed_max = std::numeric_limits<long long>::max();
        
        auto signed_max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(signed_max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(signed_max_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_FALSE(max_test);
    }
    
    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(corba2re, char2char){
    typedef char CORBA_TYPE;
    typedef RE_char RE_TYPE;
    {
        CORBA_TYPE small = 'a';
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        //Until we move to 64 bit RE integers, this will fail.
        EXPECT_TRUE(max_test);
    }
    
    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        EXPECT_TRUE(min_test);
    }
};

TEST(corba2re, wchar2char){
    typedef wchar_t CORBA_TYPE;
    typedef RE_char RE_TYPE;
    {
        CORBA_TYPE small = 'a';
        
        auto small_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(small);
        EXPECT_TRUE(small_test);
    }

    {
        CORBA_TYPE char_max = std::numeric_limits<RE_TYPE>::max();
        
        auto char_max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(char_max);
        EXPECT_TRUE(char_max_test);
    }

    {
        CORBA_TYPE max = std::numeric_limits<CORBA_TYPE>::max();
        
        auto max_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(max);
        //RE Doesn't have wchar_T support
        EXPECT_FALSE(max_test);
    }
    
    {
        CORBA_TYPE min = std::numeric_limits<CORBA_TYPE>::min();
        
        auto min_test = IsValidConversion<CORBA_TYPE, RE_TYPE>(min);
        //RE Doesn't have wchar_T support
        EXPECT_FALSE(min_test);
    }
};