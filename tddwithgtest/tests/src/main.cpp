#include <gmock/gmock.h>
#include "MyParser.h"

using namespace testing;

class ParserTest : public Test
{
};

class UniqueParserTest : public Test
{
public:
    utils::MyParser_t &_parser;

    UniqueParserTest():_parser(utils::getParser()) {}
};

TEST(ParserTest, Parser_LowerSingleLetter)
{
    std::string output = "";
    std::string input = "L";
    std::string expected = "l";
    utils::MyParser parser;
    parser.convertToLowerCase(input, output);
	ASSERT_EQ(output, "l");
}

TEST(ParserTest, Parser_UpperSingleLetter)
{
    std::string output = "";
    std::string input = "l";
    std::string expected = "L";
    utils::MyParser parser;
    parser.convertToUpperCase(input, output);
	ASSERT_EQ(output, "L");
}

TEST_F(UniqueParserTest, Parser_UniqueParserIsUnique)
{
	utils::MyParser_t& p = utils::getParser();
    ASSERT_EQ(std::addressof(p), std::addressof(_parser));
}

TEST_F(UniqueParserTest, Parser_UniqueParserLowerSingleLetter)
{
    std::string output = "";
    std::string input = "L";
    std::string expected = "l";
    _parser->convertToLowerCase(input, output);
	ASSERT_EQ(output, "l");
}

TEST_F(UniqueParserTest, Parser_UniqueParserUpperSingleLetter)
{
    std::string output = "";
    std::string input = "l";
    std::string expected = "L";
    _parser->convertToUpperCase(input, output);
	ASSERT_EQ(output, "L");
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
