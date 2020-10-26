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

MyParser_t &getParser()
{
    static MyParser_t p (new MyParser());
    return p;
}
