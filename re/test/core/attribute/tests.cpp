#include "gtest/gtest.h"

#include "attribute.h"
#include <cmath>

TEST(Re_Core_Attribute, Type_Integer){
    Attribute a(ATTRIBUTE_TYPE::INTEGER, "int");
    {
        //Test template-setter vs regular-getter and template-getter
        int value = 10;
        a.set_Integer(value);
        EXPECT_EQ(a.get_Integer(), value);
        EXPECT_EQ(a.Value<int>(), value);
    }

    {
        //Test template-setter vs regular-getter and template-getter
        int value = -100;
        a.SetValue<int>(value);
        EXPECT_EQ(a.get_Integer(), value);
        EXPECT_EQ(a.Value<int>(), value);
    }

    {
        //Test reference-template-setter vs regular-getter and template-getter
        int value = -36;
        a.SetValue<int>(value);
        EXPECT_EQ(a.get_Integer(), value);
        EXPECT_EQ(a.Value<int>(), value);

        a.Value<int>() *= value;

        EXPECT_EQ(a.get_Integer(), value * value);
        EXPECT_EQ(a.Value<int>(), value * value);
    }
    
    //Test Failure Throws
    EXPECT_ANY_THROW(a.Value<std::string>());
    EXPECT_ANY_THROW(a.Value<bool>());
    EXPECT_ANY_THROW(a.Value<double>());
    //EXPECT_ANY_THROW(a.Value<int>());
    EXPECT_ANY_THROW(a.Value<float>());
    EXPECT_ANY_THROW(a.Value<char>());
    EXPECT_ANY_THROW(a.Value<std::vector<std::string>>());
}


TEST(Re_Core_Attribute, Type_Double){
    Attribute a(ATTRIBUTE_TYPE::DOUBLE, "double");
    {
        //Test template-setter vs regular-getter and template-getter
        double value = 10.01;
        a.set_Double(value);
        EXPECT_DOUBLE_EQ(a.get_Double(), value);
        EXPECT_DOUBLE_EQ(a.Value<double>(), value);
    }

    {
        //Test template-setter vs regular-getter and template-getter
        double value = -100.001;
        a.SetValue<double>(value);
        EXPECT_DOUBLE_EQ(a.get_Double(), value);
        EXPECT_DOUBLE_EQ(a.Value<double>(), value);
    }

    {
        //Test reference-template-setter vs regular-getter and template-getter
        double value = 3.0;
        a.SetValue<double>(value);
        EXPECT_DOUBLE_EQ(a.get_Double(), value);
        EXPECT_DOUBLE_EQ(a.Value<double>(), value);

        a.Value<double>() *= value;

        EXPECT_DOUBLE_EQ(a.get_Double(), value * value);
        EXPECT_DOUBLE_EQ(a.Value<double>(), value * value);
    }
    
    //Test Failure Throws
    EXPECT_ANY_THROW(a.Value<std::string>());
    EXPECT_ANY_THROW(a.Value<bool>());
    //EXPECT_ANY_THROW(a.Value<double>());
    EXPECT_ANY_THROW(a.Value<int>());
    EXPECT_ANY_THROW(a.Value<float>());
    EXPECT_ANY_THROW(a.Value<char>());
    EXPECT_ANY_THROW(a.Value<std::vector<std::string>>());
}

TEST(Re_Core_Attribute, Type_String){
    Attribute a(ATTRIBUTE_TYPE::STRING, "string");
    {
        //Test template-setter vs regular-getter and template-getter
        std::string value{"Hello"};
        a.set_String(value);
        EXPECT_EQ(a.get_String(), value);
        EXPECT_EQ(a.Value<std::string>(), value);
    }

    {
        //Test template-setter vs regular-getter and template-getter
        std::string value{",World!"};
        a.SetValue<std::string>(value);
        EXPECT_EQ(a.get_String(), value);
        EXPECT_EQ(a.Value<std::string>(), value);
    }

    {
        //Test reference-template-setter vs regular-getter and template-getter
        std::string value{"hello hello"};
        a.SetValue<std::string>(value);
        EXPECT_EQ(a.get_String(), value);
        EXPECT_EQ(a.Value<std::string>(), value);

        a.Value<std::string>() += value;

        EXPECT_EQ(a.get_String(), value + value);
        EXPECT_EQ(a.Value<std::string>(), value + value);
    }
    
    //Test Failure Throws
    //EXPECT_ANY_THROW(a.Value<std::string>());
    EXPECT_ANY_THROW(a.Value<bool>());
    EXPECT_ANY_THROW(a.Value<double>());
    EXPECT_ANY_THROW(a.Value<int>());
    EXPECT_ANY_THROW(a.Value<float>());
    EXPECT_ANY_THROW(a.Value<char>());
    EXPECT_ANY_THROW(a.Value<std::vector<std::string>>());
}