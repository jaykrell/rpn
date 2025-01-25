#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <string>
using std::string;

string argv_join(int argc, char** argv)
{
    string a;

    for (int i = 1; i < argc; ++i)
        a.append(argv[i], argv[i] + strlen(argv[i]));

    return a;
}

string rpn(const string& input)
{
    int pluses{};
    bool times{};
    string output;

    output.reserve(input.size());

    for (const char* arg = input.c_str(); char ch = *arg; ++arg)
    {
        switch (ch)
        {
        case ' ':
            break;
        case '+':
            ++pluses;
            break;
        case '*':
            times = true;
            break;
        default:
            size_t len = strspn(arg, "0123456789");
            output.append(arg, len);
            arg += len - 1;
            output += " ";
            if (times)
            {
                output += "* ";
                times = false;
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

    assert(rpn("1 + 2 + 3 + 4") == "1 2 3 4 + + + ");
    assert(rpn("1 * 2 * 3 * 4") == "1 2 * 3 * 4 * ");
    assert(rpn("1 + 2 * 3 + 4 * 5") == "1 2 3 * 4 5 * + + ");
    assert(rpn("1 + 2 + 3 + 4 * 5 * 6 * 7 ") == "1 2 3 4 5 * 6 * 7 * + + + ");
    assert(rpn("1 * 2 * 3 * 4 + 5 + 6 + 7 ") == "1 2 * 3 * 4 * 5 6 7 + + + ");

    assert(rpn("1 + 2 + 3 + 4 * 5 * 6 * 7 + 8") == "1 2 3 4 5 * 6 * 7 * 8 + + + + ");
    assert(rpn("1 * 2 * 3 * 4 + 5 + 6 + 7 * 8") == "1 2 * 3 * 4 * 5 6 7 8 * + + + ");
}
