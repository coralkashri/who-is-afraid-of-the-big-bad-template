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
        for (size_t i = 0; i < N; ++i) {
            value[i] = str[i];
        }
    }
};

template <FixedString Name, double Price>
struct GroceryItem {
    static constexpr auto name = Name;
    static constexpr double price = Price;

    constexpr std::string_view GetName() const
    {
        return name.value;
    }
};

template <typename... Bags>
struct Cart {
    constexpr Cart(Bags... bags) : m_bags{bags...} {}
    constexpr Cart(std::tuple<Bags...> bags_tuple) : m_bags(std::move(bags_tuple)) {} // Helper for operator+

    template <typename... OtherBags>
    constexpr Cart<Bags..., OtherBags...> operator+(OtherBags... bags) const {
        return Cart<Bags..., OtherBags...>(std::tuple_cat(m_bags, std::make_tuple(bags...)));
    }

    constexpr double TotalCost() const {
        return TotalCostImpl(std::index_sequence_for<Bags...>{});
    }

    void Receipt() const {
        ReceiptImpl(std::index_sequence_for<Bags...>{});
        std::cout << "Total: " << TotalCost() << "\n";
    }

private:
    std::tuple<Bags...> m_bags;

    template <std::size_t... Is>
    constexpr double TotalCostImpl(std::index_sequence<Is...>) const {
        return (BagPrice(std::get<Is>(m_bags)) + ... + 0.0);
    }

    template <typename BagT>
    static constexpr double BagPrice(const BagT& bag) {
        return bag.size() * bag[0].price;
    }

    template <std::size_t... Is>
    void ReceiptImpl(std::index_sequence<Is...>) const {
        (([&]() {
            const auto& bag = std::get<Is>(m_bags);
            std::cout << bag[0].GetName() << "s: " << bag.size()
                      << " 💰: " << bag[0].price << "\n";
        }()), ...);
    }
};

using 🍎 = GroceryItem<"🍎", 2.99>;
using 🍌 = GroceryItem<"🍌", 5.34>;

template <typename GroceryT, int Amount>
struct Bag : public std::array<GroceryT, Amount> {
    template <typename OGroceryT, int OAmount>
    constexpr auto operator+(Bag<OGroceryT, OAmount> otherBag) const {
        return Cart(*this, otherBag);
    }
};

template <char... Digits>
static consteval int ParseAmount() {
    int res = 0;
    return ((res = res * 10 + (Digits - '0')), ...);
}

template <char... Digits>
constexpr auto operator""🛍🍎() {
    return Bag<🍎, ParseAmount<Digits...>()>{};
}

template <char... Digits>
constexpr auto operator""🛍🍌() {
    return Bag<🍌, ParseAmount<Digits...>()>{};
}

int main()
{
    constexpr auto 🍎s = 7🛍🍎;
    constexpr auto 🍌s = 10🛍🍌;
    constexpr auto 🍌s2 = 10🛍🍌;

    constexpr auto 🛒🐕 = 🍎s + 🍌s + 🍌s2;

    static_assert(🛒🐕.TotalCost() == 7 * 🍎::price + 20 * 🍌::price);

    🛒🐕.Receipt();
}

#pragma GCC diagnostic pop