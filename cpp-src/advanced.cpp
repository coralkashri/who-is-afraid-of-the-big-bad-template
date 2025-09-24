#include <iostream>
#include <vector>
#include <array>
#include <string_view>
#include <ranges>
#include <span>
#include <cmath>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"


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

template <FixedString Name, double Price>
struct GroceryItem {
    static constexpr auto name = Name;
    static constexpr double 💰 = Price;

    constexpr std::string_view GetName() const
    {
        return name.value;
    }
};

template <typename... Bags>
struct 🛒 {
    constexpr 🛒(Bags... bags) : m_bags{bags...} {}
    constexpr 🛒(std::tuple<Bags...> bags_tuple) : m_bags(std::move(bags_tuple)) {} // Helper for operator+

    template <typename... OtherBags>
    constexpr 🛒<Bags..., OtherBags...> operator+(OtherBags... bags)
    {
        return 🛒<Bags..., OtherBags...>(std::tuple_cat(m_bags, std::make_tuple(bags...)));
    }

    constexpr double 💰() const {
        return (BagPrice(std::get<Bags>(m_bags)) + ... + 0.0);
    }

    template <typename BagT>
    static constexpr double BagPrice(const BagT& bag) {
        return bag.size() * bag[0].💰;
    }

    void print() const {
        ([&]() {
            const auto& bag = std::get<Bags>(m_bags);
            std::cout << bag[0].GetName() << "s: " << bag.size() << " 💰: " << bag[0].💰 << "\n";
        }(), ...);
        
        std::cout << "Total: " << 💰() << "\n";
    }

private:
    std::tuple<Bags...> m_bags;
};

using 🍎 = GroceryItem<"🍎", 2.99>;
using 🍌 = GroceryItem<"🍌", 5.34>;

template <typename GroceryT, int Amount>
struct 🛍 : public std::array<GroceryT, Amount> {
    template <typename OGroceryT, int OAmount>
    constexpr auto operator+(🛍<OGroceryT, OAmount> otherBag) const {
        return 🛒(*this, otherBag);
    }
};

template <char... Digits>
static consteval int ParseAmount() {
    int res = 0;
    return ((res = res * 10 + (Digits - '0')), ...);
}

template <char... Digits>
constexpr auto operator""🛍🍎() {
    return 🛍<🍎, ParseAmount<Digits...>()>{};
}

template <char... Digits>
constexpr auto operator""🛍🍌() {
    return 🛍<🍌, ParseAmount<Digits...>()>{};
}

int main()
{
    constexpr auto 🍎s = 7🛍🍎;
    constexpr auto 🍌s = 10🛍🍌;
    constexpr auto 🍌s2 = 5🛍🍌;

    constexpr auto 🛒🐕 = 🍎s + 🍌s + 🍌s2;

    static_assert(🛒🐕.💰() == 7 * 🍎::💰 + 15 * 🍌::💰);

    🛒🐕.print();
}

#pragma GCC diagnostic pop