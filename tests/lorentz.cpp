#include <sstream>

#include <COLA.hh>
#include <gtest/gtest.h>

using namespace cola;

TEST(LorentzVector, Basic) {
    LorentzVector vec{
        .e=5,
        .x=1,
        .y=2,
        .z=3
    };

    EXPECT_EQ(vec.t, vec.e);

    EXPECT_EQ(vec[0], vec.e);
    EXPECT_EQ(vec[1], vec.x);
    EXPECT_EQ(vec[2], vec.y);
    EXPECT_EQ(vec[3], vec.z);

    EXPECT_EQ(vec.mag2(), 5*5 - (1*1 + 2*2 + 3*3));
    EXPECT_EQ(vec.mag(), std::sqrt(5*5 - (1*1 + 2*2 + 3*3)));
    EXPECT_TRUE(vec.isSpaceLike());
}

TEST(LorentzVector, Arithmetic) {
    LorentzVector vec1{
        .e=0,
        .x=1,
        .y=2,
        .z=3
    };

    LorentzVector vec2{
        .e=4,
        .x=3,
        .y=2,
        .z=1
    };

    {
    auto expected = LorentzVector{.e=4, 4, 4, 4};
        EXPECT_EQ(vec1 + vec2, expected);
    }

    {
        auto expected = LorentzVector{.e=-4, -2, 0, 2};
        EXPECT_EQ(vec1 - vec2, expected);
    }

    {
        auto expected = LorentzVector{.e=0, 3, 6, 9};
        EXPECT_EQ(vec1 * 3, expected);
        EXPECT_EQ(3 * vec1, expected);
    }

    {
        auto expected = LorentzVector{.e=2, 2, 2, 2};
        EXPECT_EQ((vec1 + vec2) / 2, expected);
    }

    std::stringstream ss;
    ss << vec1;
    EXPECT_EQ(ss.str(), "(0, 1, 2, 3)");
}
