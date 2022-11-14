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
    unordered_map<string, multiset<Order, BuyComparator>::iterator> buyOrders;
    unordered_map<string, multiset<Order, SellComparator>::iterator> sellOrders;

public:
    multiset<Order, BuyComparator> buyOrderbook;
    multiset<Order, SellComparator> sellOrderbook;

    void deductSellQuantity(multiset<Order, SellComparator>::iterator order, int value) {
        order->deductQuantity(value);
        if (order->getQuantity() == 0) {
            sellOrderbook.erase(sellOrderbook.begin());
            sellOrders.erase(order->getOrderId());
        }
    }

    void deductBuyQuantity(multiset<Order, BuyComparator>::iterator order, int value) {
        order->deductQuantity(value);
        if (order->getQuantity() == 0) {
            buyOrderbook.erase(buyOrderbook.begin());
            buyOrders.erase(order->getOrderId());
        }
    }

    void insertLimitOrder(Order& order) {
        // inserting the order into respective books
        if (order.isBuy()) {
            buyOrderbook.insert(order);
            buyOrders[order.getOrderId()] = buyOrderbook.find(order);
        } else {
            sellOrderbook.insert(order);
            sellOrders[order.getOrderId()] = sellOrderbook.find(order);
        }
        int totalTraded = 0;
        // match the orders if possible
        while (!buyOrderbook.empty() && !sellOrderbook.empty() 
            && buyOrderbook.begin()->getPrice() >= sellOrderbook.begin()->getPrice()) {
                auto buyOrder = buyOrderbook.begin();
                auto sellOrder = sellOrderbook.begin();
                int quantityTraded = min(buyOrder->getQuantity(), sellOrder->getQuantity());
                int priceTraded = buyOrder->getPrice();
                // remove from orderbook if quantity is depleted
                deductBuyQuantity(buyOrder, quantityTraded);
                deductSellQuantity(sellOrder, quantityTraded);
                totalTraded += quantityTraded * priceTraded;
            }   
        cout << totalTraded << "\n";
    }

    void insertMarketOrder(Order& order) {
        int totalTraded = 0;
        if (order.isBuy()) {
            while (!sellOrderbook.empty() || !order.getQuantity()) {
                auto sellOrder = sellOrderbook.begin();
                int quantityTraded = min(order.getQuantity(), sellOrder->getQuantity());
                int price = sellOrder->getPrice();
                deductSellQuantity(sellOrder, quantityTraded);
                order.deductQuantity(quantityTraded);
                totalTraded += price * quantityTraded;
            }
        } else {
            while (!buyOrderbook.empty() || !order.getQuantity()) {
                auto buyOrder = buyOrderbook.begin();
                int quantityTraded= min(order.getQuantity(), buyOrder->getQuantity());
                int price = buyOrder->getPrice();
                deductBuyQuantity(buyOrder, quantityTraded);
                order.deductQuantity(quantityTraded);
                totalTraded += price * quantityTraded;
            }
        }
        cout << totalTraded << "\n";
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
        for (auto &order : buyOrderbook) {
            cout << order << " ";
        }
        cout << "\n";
        cout << "S: ";
        for (auto &order : sellOrderbook) {
            cout << order << " ";
        }
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
            string type; ss >> type;
            string side, orderId, quantity;
            ss >> side >> orderId >> quantity;
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