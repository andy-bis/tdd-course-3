/*
We have to develop a coffee machine with TDD. The machine uses the automated source of ingredients, which implements the interface ISourceOfIngredients.

Our machine should be able to produce the coffee accordingly to the next receipts:
- americano: water & coffee 1/2 or 1/3. Water temp 60C
- cappuccino - milk & coffee & milk foam 1/3, 1/3, 1/3. Water temp 80C
- latte - milk & coffee & milk foam 1/4, 1/2, 1/4. Water temp 90C
- marochino - chocolate & coffee & milk foam, 1/4, 1/4, 1/4 and 1/4 is empty

We have 2 possible sizes of the cup:
- little 100 gram
- big 140 gram

Implement worked coffee machine using ISourceOfIngredients to controll the process of coffee production.
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

class ISourceOfIngredients
{
public:
    virtual ~ISourceOfIngredients() {}
    virtual void SetCupSize(int gram) = 0;
    virtual void AddWater(int gram, int temperature) = 0;
    virtual void AddSugar(int gram) = 0;
    virtual void AddCoffee(int gram) = 0;
    virtual void AddMilk(int gram) = 0;
    virtual void AddMilkFoam(int gram) = 0;
    virtual void AddChocolate(int gram) = 0;
    virtual void AddCream(int gram) = 0;
};


enum Cup
{
    Normal,
    Big
};

enum Coffee
{
    Americano,
    Cappuccino
};

class MockSourceOfIngredients : public ISourceOfIngredients
{
public:
    MOCK_METHOD1(SetCupSize, void(int));
    MOCK_METHOD2(AddWater, void(int, int));
    MOCK_METHOD1(AddSugar, void(int));
    MOCK_METHOD1(AddCoffee, void(int));
    MOCK_METHOD1(AddMilk, void(int));
    MOCK_METHOD1(AddMilkFoam, void(int));
    MOCK_METHOD1(AddChocolate, void(int));
    MOCK_METHOD1(AddCream, void(int));
};

class CoffeeMachine
{
public:
    CoffeeMachine(ISourceOfIngredients& source)
        : m_source(source)
    {
    }

    void CreateCoffee(const Cup cup, const Coffee coffee)
    {
        if (coffee == Coffee::Cappuccino && cup == Cup::Big)
        {
            m_source.SetCupSize(140);
            m_source.AddMilk(46);
            m_source.AddCoffee(47);
            m_source.AddMilkFoam(47);
            return;
        }

        if (coffee == Americano)
        {
            int gram = cup == Cup::Big ? 140 : 100;
            m_source.SetCupSize(gram);
            MakeAmericano(gram);
        }
        else
        {
            MakeCappuccino();
        }
    }

private:
    void MakeAmericano(int gram)
    {
        m_source.AddCoffee(gram / 2);
        m_source.AddWater(gram / 2, 60);
    }

    void MakeCappuccino()
    {
        m_source.SetCupSize(100);
        m_source.AddCoffee(33);
        m_source.AddMilk(33);
        m_source.AddMilkFoam(34);
    }

private:
    ISourceOfIngredients& m_source;
};


// Architecture
// Class CoffeMachine
// Class-Mock SourceOfIngredients

// Tests list:
// 1. americano + 100 gram = 1 coffee
// 2. americano + 140 gram = 1 large coffee
// 3. AddCoffee, SetCupSize and AddWater calls once
// 4. Check parameters
// 5. Same for each recipe

TEST(CoffeeMachine, NormalAmericano)
{
    StrictMock<MockSourceOfIngredients> si;
    CoffeeMachine cm(si);

    EXPECT_CALL(si, AddCoffee(50));
    EXPECT_CALL(si, SetCupSize(100));
    EXPECT_CALL(si, AddWater(50, 60));

    cm.CreateCoffee(Cup::Normal, Coffee::Americano);
}

TEST(CoffeeMachine, BigAmericano)
{
    StrictMock<MockSourceOfIngredients> si;
    CoffeeMachine cm(si);

    EXPECT_CALL(si, AddCoffee(70));
    EXPECT_CALL(si, SetCupSize(140));
    EXPECT_CALL(si, AddWater(70, 60));

    cm.CreateCoffee(Cup::Big, Coffee::Americano);
}

// cappuccino - milk & coffee & milk foam 1/3, 1/3, 1/3. Water temp 80C
TEST(CoffeeMachine, NormalCappuccino)
{
    StrictMock<MockSourceOfIngredients> si;
    CoffeeMachine cm(si);

    EXPECT_CALL(si, SetCupSize(100));
    EXPECT_CALL(si, AddMilk(33));
    EXPECT_CALL(si, AddCoffee(33));
    EXPECT_CALL(si, AddMilkFoam(34));

    cm.CreateCoffee(Cup::Normal, Coffee::Cappuccino);
}

TEST(CoffeeMachine, BigCappuccino)
{
    StrictMock<MockSourceOfIngredients> si;
    CoffeeMachine cm(si);

    EXPECT_CALL(si, SetCupSize(140));
    EXPECT_CALL(si, AddMilk(46));
    EXPECT_CALL(si, AddCoffee(47));
    EXPECT_CALL(si, AddMilkFoam(47));

    cm.CreateCoffee(Cup::Big, Coffee::Cappuccino);
}

