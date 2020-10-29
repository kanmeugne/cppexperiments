#include "MyParser.h"
using namespace utils;

//MyParser::convertToLowerCase
void MyParser::convertToLowerCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(tolower(c));
}
//MyParser::convertToUpperCase
void MyParser::convertToUpperCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(toupper(c));
}

//MyParser::MyParser
MyParser::MyParser()
{
}

//MyParser::~MyParser
MyParser::~MyParser()
{
}

//MyParser_t& utils::getParser()
MyParser_t& utils::getParser()
{
    static MyParser_t p = std::unique_ptr<MyParser>(new MyParser());
    return p;
}