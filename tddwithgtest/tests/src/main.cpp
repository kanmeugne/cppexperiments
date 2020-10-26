#include <gmock/gmock.h>
#include "Parser.h"

using namespace testing;

class ParserTest : public Test
{
public:
	utils::Parser_t _parser;

	void SetUp() override {
		_parser = utils::getParser();
	}
};

TEST_F(ParserTest, Parser_LowerSingleLetter)
{
    std::string output = "";
    std::string input = "L";
    _parser->convertToLowerCase(input, output)
	ASSERT_EQ(output, Eq("l"));
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
