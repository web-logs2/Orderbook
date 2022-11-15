#include <iostream>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

enum class Side {B, S};
const string LO = "LO";
const string MO = "MO";

class Order {
    Side side;
    string orderId;
    mutable int quantity;
    int price;

public:
    Order(Side s, string o, int q, int p) :
        side(s), orderId(o), quantity(q), price(p) {}

    bool isBuy() const { return side == Side::B; }
    int getPrice() const { return this->price; }
    int getQuantity() const { return this->quantity; }
    string getOrderId() const { return this->orderId; }
    void deductQuantity(int value) const { this->quantity -= value; }
};

ostream& operator<<(ostream& os, const Order& order) {
    cout << order.getQuantity() << "@" << order.getPrice() << "#" << order.getOrderId();
    return os;
}

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
    using BuyOrders = unordered_map<string, multiset<Order, BuyComparator>::iterator>;
    using SellOrders = unordered_map<string, multiset<Order, SellComparator>::iterator>;
    BuyOrders buyOrders;
    SellOrders sellOrders;

    using BuyOrderbook = multiset<Order, BuyComparator>;
    using SellOrderbook = multiset<Order, SellComparator>;
    BuyOrderbook buyOrderbook;
    SellOrderbook sellOrderbook;

public:
    template<typename T1, typename T2>
    void deductQuantity(T1& orderbook, T2& orders, int value) {
        orderbook.begin()->deductQuantity(value);
        if (!orderbook.begin()->getQuantity()) {
            orderbook.erase(orderbook.begin());
            orders.erase(orderbook.begin()->getOrderId());
        }
    }

    template<typename T1, typename T2>
    void insertToOrderbook(T1& orderbook, T2& orders, Order& order) {
        orderbook.insert(order);
        orders[order.getOrderId()] = orderbook.find(order);
    }

    void insertLimitOrder(Order& order) {
        // inserting the order into respective books
        if (order.isBuy()) insertToOrderbook<BuyOrderbook, BuyOrders>(buyOrderbook, buyOrders, order);
        else insertToOrderbook<SellOrderbook, SellOrders>(sellOrderbook, sellOrders, order);
        int totalTraded = 0;
        // match the orders if possible
        while (!buyOrderbook.empty() && !sellOrderbook.empty() 
            && buyOrderbook.begin()->getPrice() >= sellOrderbook.begin()->getPrice()) {
                auto buyOrder = buyOrderbook.begin();
                auto sellOrder = sellOrderbook.begin();
                int quantityTraded = min(buyOrder->getQuantity(), sellOrder->getQuantity());
                int priceTraded = buyOrder->getPrice();
                // remove from orderbook if quantity is depleted
                deductQuantity<BuyOrderbook, BuyOrders>(buyOrderbook, buyOrders, quantityTraded);
                deductQuantity<SellOrderbook, SellOrders>(sellOrderbook, sellOrders, quantityTraded);
                totalTraded += quantityTraded * priceTraded;
            }   
        cout << totalTraded << "\n";
    }
    
    template<typename T1, typename T2>
    int matchMarketOrders(T1& orderbook, T2& orders, Order& order) {
        int totalTraded = 0;
        while (order.getQuantity() && !orderbook.empty()) {
            int quantityTraded = min(order.getQuantity(), orderbook.begin()->getQuantity());
            int price = orderbook.begin()->getPrice();
            deductQuantity<T1, T2>(orderbook, orders, quantityTraded);
            order.deductQuantity(quantityTraded);
            totalTraded += price * quantityTraded;
        }
        return totalTraded;
    }

    void insertMarketOrder(Order& order) {
        //int totalTraded = 0;
        if (order.isBuy()) cout << matchMarketOrders<SellOrderbook, SellOrders>(sellOrderbook, sellOrders, order) << "\n";
        else cout << matchMarketOrders<BuyOrderbook, BuyOrders>(buyOrderbook, buyOrders, order) << "\n";
    }

    void cancelOrder(string orderId) {
        if (buyOrders.find(orderId) != buyOrders.end()) {
            buyOrderbook.erase(buyOrders[orderId]);
            buyOrders.erase(orderId);
        }
        else if (sellOrders.find(orderId) != sellOrders.end()) {
            sellOrderbook.erase(sellOrders[orderId]);
            sellOrders.erase(orderId);
        }
    }

    void printOrderbook() {
        cout << "B: ";
        for (auto &order : buyOrderbook) cout << order << " ";
        cout << "\nS: ";
        for (auto &order : sellOrderbook) cout << order << " ";
        cout << "\n";
    }
};

int main() {
    string input;
    Orderbook orderbook;
    while (getline(cin, input)) {
        stringstream ss(input);
        string command; ss >> command;
        if (command == "END") break;
        else if (command == "CXL") {
            string orderId; ss >> orderId;
            orderbook.cancelOrder(orderId);
        }
        else if (command == "SUB") {
            string type, side, orderId, quantity;
            ss >> type >> side >> orderId >> quantity;
            Side s = side == "B" ? Side::B : Side::S;
            if (type == "MO") {
                Order order(s, orderId, stoi(quantity), 0);
                orderbook.insertMarketOrder(order);
            }
            else if (type == "LO") {
                string price; ss >> price;
                Order order(s, orderId, stoi(quantity), stoi(price));
                orderbook.insertLimitOrder(order);
            }
        }
    } 
    orderbook.printOrderbook();
}