#include "MyParser.h"
using namespace utils;

void MyParser::convertToLowerCase(const std::string &input, std::string &output)
{
    output = "";
}
void MyParser::convertToUpperCase(const std::string &input, std::string &output)
{
    output = "";
}
MyParser::MyParser()
{
}
MyParser::~MyParser()
{
}
MyParser_t& utils::getParser()
{
    static MyParser_t p = std::unique_ptr<MyParser>(new MyParser());
    return p;
}