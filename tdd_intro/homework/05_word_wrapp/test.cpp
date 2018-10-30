/*
Write a function, that is given a string and a length limit, splits provided string into sequence of string,
where length of each string is not more, than provided limit. If there are spaces under provided limit -
last space should be used to wrapp this line. If there are no spaces - wrapp it on provided length limit.

Example:
When pos is specified, the search only includes sequences of characters that begin at or before position pos,
ignoring any possible match beginning after pos

"When pos is specified, the",
"search only includes sequences",
"of characters that begin at or",
"before position pos, ignoring",
"any possible match beginning",
"after pos."
*/

#include <gtest/gtest.h>
#include <cctype>

// empty string
// string shorter than wrap number
// word longer than wrap number
// word much longer than wrap number (more than 2 strings)
// string longer than wrap number
// wrapLength word + 1 space
// string wrapped by several whitespaces (less than wrapLength)
// string wrapped by several whitespaces (more than wrapLength)
// only whitespaces in string
// long whitespace, long word, long whitespace
// 1 space word + wrapLength word
// several words in one string
// word on line boundary

using WrappedStrings = std::vector<std::string>;

size_t GetLineLength(const std::string& str, size_t position, size_t maxLength)
{
    if (str.length() - position < maxLength)
    {
        return maxLength;
    }

    size_t startPosition = position;
    size_t lineLength = maxLength;
    for (;;)
    {
        size_t nextSpace = str.find_first_of(' ', position);
        if (nextSpace == std::string::npos)
        {
            break;
        }

        size_t newLineLength = nextSpace - startPosition;
        if (newLineLength > maxLength)
        {
            break;
        }

        lineLength = newLineLength;
        position = str.find_first_not_of(' ', nextSpace);
    }

    return lineLength;
}

WrappedStrings WrapString(const std::string& str, size_t wrapLength)
{
    WrappedStrings result;

    for (size_t i = 0;;)
    {
        i = str.find_first_not_of(' ', i);
        if (i == std::string::npos)
        {
            break;
        }

        size_t lineLength = GetLineLength(str, i, wrapLength);
        result.push_back(str.substr(i, lineLength));
        i += lineLength;
    }

    return result;
}

TEST(WrapString, EmptyString)
{
    ASSERT_EQ(WrappedStrings(), WrapString("", 25));
}

TEST(WrapString, StringShorterWrapNumber)
{
    ASSERT_EQ(WrappedStrings{"asdf"}, WrapString("asdf", 8));
}

TEST(WrapString, StringLongerThanWrapNumber)
{
    WrappedStrings expected = {"asd", "f"};
    ASSERT_EQ(expected, WrapString("asdf", 3));
}

TEST(WrapString, StringLongerThanWrapNumberSeveralParts)
{
    WrappedStrings expected = {"12", "34", "56"};
    ASSERT_EQ(expected, WrapString("123456", 2));
}

TEST(WrapString, MultipleWordsLonger)
{
    WrappedStrings expected = {"1", "2"};
    ASSERT_EQ(expected, WrapString("1 2", 1));
}

TEST(WrapString, SpaceStringEnd)
{
    WrappedStrings expected = {"1", "2"};
    ASSERT_EQ(expected, WrapString("1 2", 2));
}

TEST(WrapString, StringWrappedBySeveralWhitespace)
{
    WrappedStrings expected = {"12", "34"};
    ASSERT_EQ(expected, WrapString("12  34", 3));
}

TEST(WrapString, WrapLengthWord1Space)
{
    WrappedStrings expected = {"123"};
    ASSERT_EQ(expected, WrapString("123 ", 3));
}

TEST(WrapString, StringWrappedBySeveralWhitespaceLong)
{
    WrappedStrings expected = {"12", "34"};
    ASSERT_EQ(expected, WrapString("12    34", 3));
}

TEST(WrapString, SeveralEndWhitespaces)
{
    WrappedStrings expected = {"12", "34"};
    ASSERT_EQ(expected, WrapString("12    34", 5));
}

TEST(WrapString, OnlyWhitespace)
{
    WrappedStrings expected;
    ASSERT_EQ(expected, WrapString("          ", 3));
}

TEST(WrapString, LongSequences)
{
    WrappedStrings expected = {"123", "4", "123", "45"};
    ASSERT_EQ(expected, WrapString("1234     12345", 3));
}

TEST(WrapString, SpaceAndWrapLengthWord)
{
    WrappedStrings expected = {"123"};
    ASSERT_EQ(expected, WrapString(" 123", 3));
}

TEST(WrapString, SeveralWordsInOneString)
{
    WrappedStrings expected = {"123 567 9", "abc"};
    ASSERT_EQ(expected, WrapString("123 567 9  abc", 10));
}

TEST(WrapString, WordOnLineBoundary)
{
    WrappedStrings expected = {"123", "567"};
    ASSERT_EQ(expected, WrapString("123 567", 5));
}

TEST(WrapString, SeveralWordsInOnlyOneString)
{
    WrappedStrings expected = {"123 567 9"};
    ASSERT_EQ(expected, WrapString("123 567 9", 10));
}

/*TEST(WrapString, Acceptance)
{
    WrappedStrings expected = {
        "When pos is specified, the",
        "search only includes sequences",
        "of characters that begin at or",
        "before position pos, ignoring",
        "any possible match beginning",
        "after pos."
    };

    std::string data = "When pos is specified, the search only includes sequences of characters that begin at or before position pos, ignoring any possible match beginning after pos.";
    ASSERT_EQ(expected, WrapString(data, 30));
}*/
