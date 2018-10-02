/*
Given a year, report if it is a leap year.

The tricky thing here is that a leap year in the Gregorian calendar occurs:

on every year that is evenly divisible by 4
  except every year that is evenly divisible by 100
    unless the year is also evenly divisible by 400

For example, 1997 is not a leap year, but 1996 is. 1900 is not a leap year, but 2000 is.

If your language provides a method in the standard library that does this look-up, pretend it doesn't exist and implement it yourself.
*/

#include <gtest/gtest.h>

bool IsLeapYear(unsigned int year)
{
    if ((year % 100) == 0)
    {
        return false;
    }

    return (year % 4) == 0;
}

TEST(IsLeapYear, RegularYear)
{
    EXPECT_FALSE(IsLeapYear(1997));
}

TEST(IsLeapYear, SimpleLeap)
{
    EXPECT_TRUE(IsLeapYear(1996));
}

TEST(IsLeapYear, Hundredth)
{
    EXPECT_FALSE(IsLeapYear(1900));
}

TEST(IsLeapYear, FourHundredth)
{
    EXPECT_TRUE(IsLeapYear(2000));
}
