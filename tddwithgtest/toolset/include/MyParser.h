#ifndef MYPARSER_H
#include <string>
#include <memory>

namespace utils
{
    class MyParser
    {
    public:
        MyParser();
        ~MyParser();

        void convertToLowerCase(const std::string &, std::string &);
        void convertToUpperCase(const std::string &, std::string &);
    };

    typedef std::unique_ptr<MyParser> MyParser_t;

    MyParser_t& getParser();
} // namespace utils
#endif // MYPARSER_H