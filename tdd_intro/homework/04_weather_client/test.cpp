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
double GetAverage(const std::vector<Weather>& weathers, T Weather::*member)
{
    if (weathers.empty())
    {
        return 0;
    }

    double sum = 0;

    for (const Weather& weather : weathers)
    {
        sum += weather.*member;
    }

    return sum / weathers.size();
}

template <class T>
T GetMinimum(const std::vector<Weather>& weathers, T Weather::*member)
{
    if (weathers.empty())
    {
        return T();
    }

    T minimum = std::numeric_limits<T>::max();
    for (const Weather& weather : weathers)
    {
        minimum = std::min(minimum, weather.*member);
    }

    return minimum;
}

template <class T>
T GetMaximum(const std::vector<Weather>& weathers, T Weather::*member)
{
    if (weathers.empty())
    {
        return T();
    }

    T minimum = std::numeric_limits<T>::min();
    for (const Weather& weather : weathers)
    {
        minimum = std::max(minimum, weather.*member);
    }

    return minimum;
}

TEST(GetAverage, WeatherWindDirection_EmptyList)
{
    std::vector<Weather> weathers = {};
    EXPECT_DOUBLE_EQ(GetAverage(weathers, &Weather::windDirection), 0);
}

TEST(GetAverage, WeatherWindDirection_Correct4Values)
{
    std::vector<Weather> weathers = {
        Weather{21, 158, 3.8},
        Weather{25, 201, 3.5},
        Weather{34, 258, 3.7},
        Weather{27, 299, 4.0}
    };

    EXPECT_DOUBLE_EQ(GetAverage(weathers, &Weather::windDirection), 229);
}

TEST(GetMinimum, WeatherTemperature_EmptyList)
{
    std::vector<Weather> weathers = {};
    EXPECT_EQ(GetMinimum(weathers, &Weather::temperature), 0);
}

TEST(GetMinimum, WeatherTemperature_Correct4Values)
{
    std::vector<Weather> weathers = {
        Weather{-5, 181, 5.1},
        Weather{5, 204, 4.9},
        Weather{10, 193, 4.3},
        Weather{15, 179, 4.5}
    };

    EXPECT_EQ(GetMinimum(weathers, &Weather::temperature), -5);
}

TEST(GetMinimum, WeatherTemperature_Correct4ValuesGreaterThan0)
{
    std::vector<Weather> weathers = {
        Weather{11, 181, 5.1},
        Weather{12, 204, 4.9},
        Weather{33, 193, 4.3},
        Weather{8, 179, 4.5}
    };

    EXPECT_EQ(GetMinimum(weathers, &Weather::temperature), 8);
}

TEST(GetMaximum, WeatherTemperature_EmptyList)
{
    std::vector<Weather> weathers = {};
    EXPECT_EQ(GetMaximum(weathers, &Weather::temperature), 0);
}

TEST(GetMaximum, WeatherTemperature_Correct4Values)
{
    std::vector<Weather> weathers = {
        Weather{5, 181, 5.1},
        Weather{10, 204, 4.9},
        Weather{15, 193, 4.3},
        Weather{20, 179, 4.5}
    };

    EXPECT_EQ(GetMaximum(weathers, &Weather::temperature), 20);
}

TEST(GetMaximum, WeatherTemperature_Correct4ValuesLessThan0)
{
    std::vector<Weather> weathers = {
        Weather{-4, 181, 5.1},
        Weather{-2, 204, 4.9},
        Weather{-1, 193, 4.3},
        Weather{-3, 179, 4.5}
    };

    EXPECT_EQ(GetMaximum(weathers, &Weather::temperature), -1);
}

// --------------------------------------------------
// WeatherClient tests
// for each function GetAverageTemperature, GetMinimumTemperature, GetMaximumTemperature, GetAverageWindDirection, GetMaximumWindSpeed
// 1 acceptance test with correct date
// will not be covered:
// incorrect date or input

class WeatherClient : public IWeatherClient
{
public:
    virtual double GetAverageTemperature(IWeatherServer& server, const std::string& date) override
    {
        std::vector<Weather> weathers = GetWeathersByDate(server, date);
        return GetAverage(weathers, &Weather::temperature);
    }

    virtual double GetMinimumTemperature(IWeatherServer& server, const std::string& date) override
    {
        std::vector<Weather> weathers = GetWeathersByDate(server, date);
        return GetMinimum(weathers, &Weather::temperature);
    }

    virtual double GetMaximumTemperature(IWeatherServer& server, const std::string& date) override
    {
        std::vector<Weather> weathers = GetWeathersByDate(server, date);
        return GetMaximum(weathers, &Weather::temperature);
    }

    virtual double GetAverageWindDirection(IWeatherServer& server, const std::string& date) override
    {
        std::vector<Weather> weathers = GetWeathersByDate(server, date);
        return GetAverage(weathers, &Weather::windDirection);
    }

    virtual double GetMaximumWindSpeed(IWeatherServer& server, const std::string& date) override
    {
        return 0;
    }
};

TEST(WeatherClient, GetAverageTemperature_CorrectDate)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("31.08.2018;03:00")).WillOnce(Return("20;181;5.1"));
    EXPECT_CALL(server, GetWeather("31.08.2018;09:00")).WillOnce(Return("23;204;4.9"));
    EXPECT_CALL(server, GetWeather("31.08.2018;15:00")).WillOnce(Return("33;193;4.3"));
    EXPECT_CALL(server, GetWeather("31.08.2018;21:00")).WillOnce(Return("26;179;4.5"));

    WeatherClient client;
    EXPECT_DOUBLE_EQ(client.GetAverageTemperature(server, "31.08.2018"), 25.5);
}

TEST(WeatherClient, GetMinimumTemperature_CorrectDate)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("01.09.2018;03:00")).WillOnce(Return("19;176;4.2"));
    EXPECT_CALL(server, GetWeather("01.09.2018;09:00")).WillOnce(Return("22;131;4.1"));
    EXPECT_CALL(server, GetWeather("01.09.2018;15:00")).WillOnce(Return("31;109;4.0"));
    EXPECT_CALL(server, GetWeather("01.09.2018;21:00")).WillOnce(Return("24;127;4.1"));

    WeatherClient client;
    EXPECT_DOUBLE_EQ(client.GetMinimumTemperature(server, "01.09.2018"), 19);
}

TEST(WeatherClient, GetMaximumTemperature_CorrectDate)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("02.09.2018;03:00")).WillOnce(Return("21;158;3.8"));
    EXPECT_CALL(server, GetWeather("02.09.2018;09:00")).WillOnce(Return("25;201;3.5"));
    EXPECT_CALL(server, GetWeather("02.09.2018;15:00")).WillOnce(Return("34;258;3.7"));
    EXPECT_CALL(server, GetWeather("02.09.2018;21:00")).WillOnce(Return("27;299;4.0"));

    WeatherClient client;
    EXPECT_DOUBLE_EQ(client.GetMaximumTemperature(server, "02.09.2018"), 34);
}

TEST(WeatherClient, GetAverageWindDirection_CorrectDate)
{
    FakeWeatherServer server;
    EXPECT_CALL(server, GetWeather("31.08.2018;03:00")).WillOnce(Return("20;181;5.1"));
    EXPECT_CALL(server, GetWeather("31.08.2018;09:00")).WillOnce(Return("23;204;4.9"));
    EXPECT_CALL(server, GetWeather("31.08.2018;15:00")).WillOnce(Return("33;193;4.3"));
    EXPECT_CALL(server, GetWeather("31.08.2018;21:00")).WillOnce(Return("26;179;4.5"));

    WeatherClient client;
    EXPECT_DOUBLE_EQ(client.GetAverageWindDirection(server, "31.08.2018"), 189.25);
}
