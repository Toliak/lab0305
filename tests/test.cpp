#include <gtest/gtest.h>
#include <boost/regex.hpp>
#include "BrokerResolver.h"

TEST(Test, Test)
{
    boost::regex r{R"((\w+)_(\d{8})_([12]\d{3}(0[1-9]|1[0-2])(0[1-9]|[12]\d|3[01])).(\w+))"};
    std::string tester{"builder_12345678_20190504.txt"};

    EXPECT_EQ(
        static_cast<bool>(boost::regex_match(tester, r)),
        true
    );
}

TEST(Test, Test1)
{
    BrokerData::Pair::Hash{}(
        BrokerData::Pair{
            "a", 1

        }
    );
}