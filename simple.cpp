#include <iostream>
#include <vector>
#include <array>
#include <string_view>
#include <ranges>

template <size_t N>
struct FixedString {
    char value[N];

    constexpr FixedString(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i)
            value[i] = str[i];
    }

    constexpr bool operator==(const FixedString& other) const {
        for (size_t i = 0; i < N; ++i)
            if (value[i] != other.value[i]) return false;
        return true;
    }
};

struct IGrocery {
    virtual constexpr std::string_view GetName() const = 0;
    virtual constexpr double GetPrice() const = 0;
    virtual constexpr bool operator==(const IGrocery& ref) const = 0;
    constexpr bool operator!=(const IGrocery& ref) const = default;
};

template <FixedString Name, double Price>
struct GroceryItem : IGrocery {
    static constexpr auto name = Name;
    static constexpr double price = Price;

    constexpr std::string_view GetName() const override
    {
        return name.value;
    }

    constexpr double GetPrice() const override
    {
        return price;
    }

    constexpr bool operator==(const IGrocery& ref) const override
    {
        return ref.GetPrice() == GetPrice() && ref.GetName() == GetName();
    }
};

struct Cart {
    Cart(const Cart&) = default;
    Cart& operator=(const Cart&) = default;

    template <typename... Items>
    Cart(std::vector<Items>&... items) {
        auto list = {(items | std::views::transform([](auto& i) { return &i; }) | std::ranges::to<std::vector<IGrocery*>>())...};
        for (auto l : list)
        {
            m_items.insert(m_items.end(), l.begin(), l.end());
        }
    }

    template <typename... Items>
    Cart& operator+=(std::vector<Items>&... items) {
        auto list = {(items | std::views::transform([](auto& i) { return &i; }) | std::ranges::to<std::vector<IGrocery*>>())...};
        for (auto l : list)
        {
            m_items.insert(m_items.end(), l.begin(), l.end());
        }
        return *this;
    }

    template <typename... Items>
    Cart& operator+=(const Cart& otherCart) {
        m_items.insert(m_items.end(), otherCart.m_items.begin(), otherCart.m_items.end());
        return *this;
    }

    template <typename Bag>
    Cart operator+(Bag& bag)
    {
        Cart temp = *this;
        temp += bag;
        return temp;
    }

    void PrintCart()
    {
        double totalPrice = 0.0;
        IGrocery* lastItem = m_items[0];
        int currentItemCounter = 0;

        auto finalizeItem = [](IGrocery* itemType, int amount)
        {
            std::cout << itemType->GetName() << "s Count: " << amount << " Price: " << itemType->GetPrice() << "\n";
            return amount * itemType->GetPrice();
        };

        for (auto* item : m_items)
        {
            if (*lastItem != *item)
            {
                totalPrice += finalizeItem(lastItem, currentItemCounter);
                lastItem = item;
                currentItemCounter = 0;
            }
            ++currentItemCounter;
        }

        totalPrice += finalizeItem(lastItem, currentItemCounter);

        std::cout << "Total: " << totalPrice << "\n";
    }

private:
    std::vector<IGrocery*> m_items;
};

using Apple = GroceryItem<"apple", 2.99>;
using Banana = GroceryItem<"banana", 5.34>;

template <typename GroceryT>
struct Bag : public std::vector<GroceryT>{
    Bag(unsigned long long n) : std::vector<GroceryT>(n) {}

    template <typename OGroceryT>
    Cart operator+(Bag<OGroceryT>& otherBag)
    {
        return Cart(*this, otherBag);
    }
};

auto operator""🍎(unsigned long long n)
{
    return Bag<Apple>(n);
}

auto operator""🍌(unsigned long long n)
{
    return Bag<Banana>(n);
}

int main()
{
    auto 🍎s = 7🍎;
    auto 🍌s = 10🍌;

    auto 🛒 = 🍎s + 🍌s + 🍌s;

    🛒.PrintCart();
}