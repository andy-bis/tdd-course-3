/*
Weather Client

You are going to develop a program that gets the statistics about weather in the current city
using information from a certain server. The goal is to calculate statistics using the data from weather server.

To communicate with the weather server you have to implement interface IWeatherServer,
which provides the raw string from the real server for the requested day and time.

The real server (i.e. "weather.com") gets the requests in this format:
"<date>;<time>", for example:
"31.08.2018;03:00"

The server answers on requests with string like this:
"20;181;5.1"
This string contains the weather for the requested time and means next:
"<air_temperature_in_celsius>;<wind_direction_in_degrees>:<wind_speed>".
Wind direction value may be in range from 0 to 359 inclusively, temperature may be negative.

The task:
1. Implement fake server, because interacting with real network is inacceptable within the unit tests.
To do this, you need to use real server responses. Fortunately, you've collected some results for the several dates from the weather server.
Each line means "<request>" : "<response>":

"31.08.2018;03:00" : "20;181;5.1"
"31.08.2018;09:00" : "23;204;4.9"
"31.08.2018;15:00" : "33;193;4.3"
"31.08.2018;21:00" : "26;179;4.5"

"01.09.2018;03:00" : "19;176;4.2"
"01.09.2018;09:00" : "22;131;4.1"
"01.09.2018;15:00" : "31;109;4.0"
"01.09.2018;21:00" : "24;127;4.1"

"02.09.2018;03:00" : "21;158;3.8"
"02.09.2018;09:00" : "25;201;3.5"
"02.09.2018;15:00" : "34;258;3.7"
"02.09.2018;21:00" : "27;299;4.0"

IMPORTANT:
* Server returns empty string if request is invalid.
* Real server stores weather only for times 03:00, 09:00, 15:00 and 21:00 for every date. Do not use other hours in a day.

2. Implement IWeatherClient using fake server.
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

struct Weather
{
    short temperature;
    unsigned short windDirection;
    double windSpeed;
    bool operator==(const Weather& right) const
    {
        return temperature == right.temperature &&
               windDirection == right.windDirection &&
               std::abs(windSpeed - right.windSpeed) < 0.01;
    }
};

class IWeatherServer
{
public:
    virtual ~IWeatherServer() { }
    // Returns raw response with weather for the given day and time in request
    virtual std::string GetWeather(const std::string& request) = 0;
};

// Implement this interface
class IWeatherClient
{
public:
    virtual ~IWeatherClient() { }
    virtual double GetAverageTemperature(IWeatherServer& server, const std::string& date) = 0;
    virtual double GetMinimumTemperature(IWeatherServer& server, const std::string& date) = 0;
    virtual double GetMaximumTemperature(IWeatherServer& server, const std::string& date) = 0;
    virtual double GetAverageWindDirection(IWeatherServer& server, const std::string& date) = 0;
    virtual double GetMaximumWindSpeed(IWeatherServer& server, const std::string& date) = 0;
};

// steps to implement
// ParseWeather - from string to struct
// GetWeathersByDate - from server to weather list
// GetAverage, GetMinimum, GetMaximum - from template list of structs to value
// GetAverageTemperature, GetMinimumTemperature, GetMaximumTemperature, GetAverageWindDirection, GetMaximumWindSpeed - from weather list to value
// WeatherClient - combining utilities to get results

// --------------------------------------------------
// ParseWeather tests
// correct weather
// empty string
// will not be covered/implemented:
// invalid temperature (< -273), invalid wind direction (>359), invalid wind speed (<0)
// any other invalid input: several whitespaces, invalid semicolons, not enough numbers etc

Weather ParseWeather(const std::string& value)
{
    if (value.empty())
    {
        throw std::invalid_argument("Empty weather string");
    }

    Weather weather = {};
    std::istringstream stream(value);
    stream >> weather.temperature;
    stream.ignore();
    stream >> weather.windDirection;
    stream.ignore();
    stream >> weather.windSpeed;
    return weather;
}

TEST(ParseWeather, Correct)
{
    Weather expected = {20, 181, 5.1};
    EXPECT_EQ(expected, ParseWeather("20;181;5.1"));
}

TEST(ParseWeather, AnotherCorrect)
{
    Weather expected = {34, 258, 3.7};
    EXPECT_EQ(expected, ParseWeather("34;258;3.7"));
}

TEST(ParseWeather, EmptyString)
{
    EXPECT_THROW(ParseWeather(""), std::invalid_argument);
}

// --------------------------------------------------
class FakeWeatherServer : public IWeatherServer
{
public:
    MOCK_METHOD1(GetWeather, std::string(const std::string& request));
};

// --------------------------------------------------
// GetWeathersByDate tests
// correct date
// incorrect date
// incorect date input
// will not be covered/implemented:
// server internal error

std::vector<Weather> GetWeathersByDate(IWeatherServer& server, const std::string& date)
{
    std::vector<Weather> weathers;

    static const char* times[] = {"03:00", "09:00", "15:00", "21:00"};
    for (const char* time : times)
    {
        std::string request = date + ";" + time;
        std::string response = server.GetWeather(request);
        weathers.push_back(ParseWeather(response));
    }

    return weathers;
}

TEST(GetWeathersByDate, CorrectDate)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("31.08.2018;03:00")).WillOnce(Return("20;181;5.1"));
    EXPECT_CALL(server, GetWeather("31.08.2018;09:00")).WillOnce(Return("23;204;4.9"));
    EXPECT_CALL(server, GetWeather("31.08.2018;15:00")).WillOnce(Return("33;193;4.3"));
    EXPECT_CALL(server, GetWeather("31.08.2018;21:00")).WillOnce(Return("26;179;4.5"));

    ASSERT_THAT(GetWeathersByDate(server, "31.08.2018"), ElementsAre(
        Weather{20, 181, 5.1},
        Weather{23, 204, 4.9},
        Weather{33, 193, 4.3},
        Weather{26, 179, 4.5}
    ));
}

TEST(GetWeathersByDate, AnotherCorrectDate)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("02.09.2018;03:00")).WillOnce(Return("21;158;3.8"));
    EXPECT_CALL(server, GetWeather("02.09.2018;09:00")).WillOnce(Return("25;201;3.5"));
    EXPECT_CALL(server, GetWeather("02.09.2018;15:00")).WillOnce(Return("34;258;3.7"));
    EXPECT_CALL(server, GetWeather("02.09.2018;21:00")).WillOnce(Return("27;299;4.0"));

    ASSERT_THAT(GetWeathersByDate(server, "02.09.2018"), ElementsAre(
        Weather{21, 158, 3.8},
        Weather{25, 201, 3.5},
        Weather{34, 258, 3.7},
        Weather{27, 299, 4.0}
    ));
}

TEST(GetWeathersByDate, IncorrectDate)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("32.08.2018;03:00")).WillOnce(Return(""));

    EXPECT_THROW(GetWeathersByDate(server, "32.08.2018"), std::invalid_argument);
}

TEST(GetWeathersByDate, IncorrectDateInput)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("31.08.2018; DROP TABLE Weather;;03:00")).WillOnce(Return(""));

    EXPECT_THROW(GetWeathersByDate(server, "31.08.2018; DROP TABLE Weather;"), std::invalid_argument);
}

// --------------------------------------------------
// GetAverage, GetMinimum, GetMaximum
// test list will be very similar for each function
// empty list
// 4 correct Weather values in list, wind direction
// 4 correct Weather values in list where min value is > 0, temperature (only for GetMinimum)
// 4 correct Weather values in list where max value is < 0, temperature (only for GetMaximum)
// will not be covered/implemented:
// 1 value in list
// integer overflow
// float precision checking
// other struct fields/methods
// template structs
// template numberic values

template <class T>
float GetAverage(const std::vector<Weather>& weathers, T Weather::*member)
{
    if (weathers.empty())
    {
        return 0;
    }

    float sum = 0;

    for (const Weather& weather : weathers)
    {
        sum += weather.*member;
    }

    return sum / weathers.size();
}

TEST(GetAverage, WeatherWindDirection_EmptyList)
{
    std::vector<Weather> weathers = {};
    EXPECT_FLOAT_EQ(GetAverage(weathers, &Weather::windDirection), 0);
}

TEST(GetAverage, WeatherWindDirection_Correct4Values)
{
    std::vector<Weather> weathers = {
        Weather{21, 158, 3.8},
        Weather{25, 201, 3.5},
        Weather{34, 258, 3.7},
        Weather{27, 299, 4.0}
    };

    EXPECT_FLOAT_EQ(GetAverage(weathers, &Weather::windDirection), 229);
}

// --------------------------------------------------
// GetAverageTemperature, GetMinimumTemperature, GetMaximumTemperature, GetAverageWindDirection, GetMaximumWindSpeed
// test list will be the very similar for each function
// 4 correct Weather values in list
// will not be covered/implemented:
// everything that mentioned in GetAverage/Minimum/Maximum tests
// input validation like invalid temperature (< -273), invalid wind direction (>359), invalid wind speed (<0)

float GetAverageTemperature(const std::vector<Weather>& weathers)
{
    if (weathers.empty())
    {
        return 0;
    }

    float sum = 0;

    for (const Weather& weather : weathers)
    {
        sum += weather.temperature;
    }

    return sum / weathers.size();
}

TEST(GetAverageTemperature, EmptyList)
{
    EXPECT_FLOAT_EQ(GetAverageTemperature({}), 0);
}

TEST(GetAverageTemperature, Correct4Values)
{
    std::vector<Weather> weathers = {
        Weather{21, 158, 3.8},
        Weather{25, 201, 3.5},
        Weather{34, 258, 3.7},
        Weather{27, 299, 4.0}
    };

    EXPECT_FLOAT_EQ(GetAverageTemperature(weathers), 26.75);
}
