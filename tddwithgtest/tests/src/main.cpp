#include <gmock/gmock.h>
#include "MyParser.h"

using namespace testing;

class ParserTest : public Test{};

TEST(ParserTest, Parser_LowerSingleLetter)
{
    std::string output = "";
    std::string input = "L";
    std::string expected = "l";
    utils::MyParser_t& parser = utils::getParser();
    parser->convertToLowerCase(input, output);
	ASSERT_EQ(output, "l");
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
