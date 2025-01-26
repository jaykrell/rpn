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
    size_t pluses{};
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
            // Defer any number of pluses to the end, i.e. without
            // knowing how many multiplies are ahead.
            ++pluses;
            break;
        case '*':
            // Swap * and the next token.
            times = true;
            ++arg;
            while (*arg == ' ') ++arg;
        default:
            size_t len = strcspn(arg, "+* ");
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
    for (size_t i = 0; i < pluses; ++i)
        output += "+ ";
    return output;
}

size_t count_chars(const string& input, char ch)
{
    size_t count{};
    const char* p = input.c_str();
    while (p = strchr(p, ch))
    {
        ++count;
        ++p;
    }
    return count;
}

string rpn_opt(const string& input)
{
    // Use less stack in evaluation.

    size_t pluses{};
    char defer{};
    string output;

    size_t multiplies_remaining = count_chars(input, '*');

    output.reserve(input.size());

    for (const char* arg = input.c_str(); char ch = *arg; ++arg)
    {
        switch (ch)
        {
        case ' ':
            break;
        case '+':
            // Defer any number of pluses to after all multiplies.
            if (multiplies_remaining)
            {
                ++pluses;
                break;
            }
        case '*':
            // Swap * and the next token.
            defer = ch;
            ++arg;
            while (*arg == ' ') ++arg;
        default:
            size_t len = strcspn(arg, "+* ");
            output.append(arg, len);
            arg += len - 1;
            output += " ";
            if (defer)
            {
                output += defer;
                output += ' ';
                defer = 0;
            }
            if (multiplies_remaining && ch == '*')
            {
                --multiplies_remaining;
                if (multiplies_remaining == 0)
                {
                    for (size_t i = 0; i < pluses; ++i)
                        output += "+ ";
                    pluses = 0;
                }
            }
            break;
        }
    }
    for (size_t i = 0; i < pluses; ++i)
        output += "+ ";
    return output;
}

int main(int argc, char** argv)
{
    string in = argv_join(argc, argv);
    int errors{};
    string out;
    string opt;

    if (in.size())
    {
        out = rpn(in);
        opt = rpn_opt(in);

        printf("%s => %s\n", in.c_str(), out.c_str());
        printf("opt %s => %s\n", in.c_str(), opt.c_str());
    }

    assert(count_chars("", 0) == 0);
    assert(count_chars("", 'a') == 0);

    assert(count_chars("a", 0) == 0);
    assert(count_chars("a", 'a') == 1);
    assert(count_chars("A", 'a') == 0);

    assert(count_chars("aa", 'A') == 0);
    assert(count_chars("aa", 0) == 0);
    assert(count_chars("aa", 'a') == 2);
    assert(count_chars("Aa", 'a') == 1);
    assert(count_chars("aA", 'a') == 1);
    assert(count_chars("AA", 'a') == 0);

    assert(count_chars("AAA", 'a') == 0);
    assert(count_chars("aAA", 'a') == 1);
    assert(count_chars("AaA", 'a') == 1);
    assert(count_chars("AAa", 'a') == 1);

    assert(count_chars("AAA", 'a') == 0);
    assert(count_chars("aaA", 'a') == 2);
    assert(count_chars("Aaa", 'a') == 2);
    assert(count_chars("aAa", 'a') == 2);

    assert(count_chars("aaAA", 'a') == 2);
    assert(count_chars("AaaA", 'a') == 2);
    assert(count_chars("AAaa", 'a') == 2);
    assert(count_chars("aAaA", 'a') == 2);
    assert(count_chars("AaAa", 'a') == 2);
    assert(count_chars("aAAa", 'a') == 2);

    static const struct {
        const char* in;
        const char* expected;
        const char* expected_opt;
    } tests[] = {
        /*0*/ {"1+2*3", "1 2 3 * + "},
        /*1*/ {"1*2+3", "1 2 * 3 + "},
        /*2*/ {"1 + 2 + 3 + 4", "1 2 3 4 + + + ", "1 2 + 3 + 4 + "},
        /*3*/ {"1 * 2 * 3 * 4", "1 2 * 3 * 4 * "},
        /*4*/ {"1 + 2 * 3 + 4 * 5", "1 2 3 * 4 5 * + + "},
        /*5*/ {"1 + 2 + 3 + 4 * 5 * 6 * 7 ", "1 2 3 4 5 * 6 * 7 * + + + "},
        /*6*/ {"1 * 2 * 3 * 4 + 5 + 6 + 7 ", "1 2 * 3 * 4 * 5 6 7 + + + ", "1 2 * 3 * 4 * 5 + 6 + 7 + "},
        /*7*/ {"1 + 2 + 3 + 4 * 5 * 6 * 7 + 8", "1 2 3 4 5 * 6 * 7 * 8 + + + + ", "1 2 3 4 5 * 6 * 7 * + + + 8 + "},
        /*8*/ {"1 * 2 * 3 * 4 + 5 + 6 + 7 * 8", "1 2 * 3 * 4 * 5 6 7 8 * + + + "},
        /*9*/ {"A+bc*3", "A bc 3 * + "},
    };

    int t{};

    for (const auto& test: tests)
    {
        out = rpn(test.in);
        opt = rpn_opt(test.in);
        bool success = (out == test.expected);
        errors += !success;
        printf("%d %s: %s => %s\n", t, success ? "success" : "fail", test.in, out.c_str());

        success = (opt == (test.expected_opt ? test.expected_opt : test.expected));
        errors += !success;
        printf("%d opt %s: %s => %s\n", t, success ? "success" : "fail", test.in, opt.c_str());
        ++t;
    }
    printf("errors:%d\n", errors);
    return errors;
}
