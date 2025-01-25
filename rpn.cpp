#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
using std::string;

std::string argv_join(int argc, char** argv)
{
    std::string a;

    for (int i = 1; i < argc; ++i)
        a.append(argv[i], argv[i] + strlen(argv[i]));

    return a;
}

std::string rpn(const std::string& input)
{
    int pluses{};
    int times{};
    int value{};
    string output;

    output.reserve(input.size());

    for (const char* arg = input.c_str(); char ch = *arg;)
    {
        switch (ch)
        {
        case ' ':
            ++arg;
            break;
        case '+':
            ++arg;
            ++pluses;
            break;
        case '*':
            ++arg;
            ++times;
            break;
        default:
            value = strtol(arg, const_cast<char**>(&arg), 10);
            output += std::to_string(value);
            output += " ";
            if (times)
            {
                output += "* ";
                --times;
            }
            break;
        }
    }
    while (pluses--)
        output += "+ ";
    printf("%s\n", output.c_str());
    return output;
}

int main(int argc, char** argv)
{
    string input = argv_join(argc, argv);

    string output = rpn(input);

    assert(rpn("1+2*3") == "1 2 3 * + ");
    assert(rpn("1*2+3") == "1 2 * 3 + ");
}
