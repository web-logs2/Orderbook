#include "Order.hpp"

class BuyComparator {
public:
    bool operator()(const Order& o1, const Order& o2) const {
        return o1.getPrice() > o2.getPrice();
    }
};

class SellComparator {
public:
    bool operator()(const Order& o1, const Order& o2) const {
        return o1.getPrice() < o2.getPrice();
    }
};

class Orderbook {
private:
    using BuyOrderMap = std::unordered_map<std::string, std::multiset<Order, BuyComparator>::iterator>;
    using SellOrderMap = std::unordered_map<std::string, std::multiset<Order, SellComparator>::iterator>;
    BuyOrderMap buyOrderMap;
    SellOrderMap sellOrderMap;

    using BuyOrderbook = std::multiset<Order, BuyComparator>;
    using SellOrderbook = std::multiset<Order, SellComparator>;
    BuyOrderbook buyOrderbook;
    SellOrderbook sellOrderbook;

    template<typename T1, typename T2>
    void deductQuantity(T1& orderbook, T2& orders, int value);

    template<typename T1, typename T2>
    int matchMarketOrders(T1& orderbook, T2& orders, Order& order);

    template<typename T1, typename T2>
    void cancelOrderInOrderMap(std::string orderId, T1& orderbook, T2& ordermap);

    template<typename T1, typename T2>
    void insertToOrderbook(T1& orderbook, T2& orders, Order& order);

public:
    void InsertLimitOrder(Order& order);

    void InsertMarketOrder(Order& order);

    void CancelOrder(std::string orderId);

    void PrintOrderbook();
};