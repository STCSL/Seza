#include <gtest/gtest.h>

#include <JsonDefinitions.h>
#include <JsonSerializer.h>
#include <JsonDeserializer.h>

TEST(NullTest, StreamJSONTest)
{

EXPECT_EQ(2 + 2, 4);
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
