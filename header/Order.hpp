#include "../include/includes.hpp"

enum class Side {B, S};

const std::string LimitOrder = "LO";
const std::string MarketOrder = "MO";

class Order {
    Side side;
    std::string orderId;
    mutable int quantity;
    int price;

public:
    Order(Side s, std::string o, int q, int p) : side(s), orderId(o), quantity(q), price(p) {}

    bool isBuy() const { return side == Side::B; }
    int getPrice() const { return this->price; }
    int getQuantity() const { return this->quantity; }
    std::string getOrderId() const { return this->orderId; }
    void deductQuantity(int value) const { this->quantity -= value; }
};

std::ostream& operator<<(std::ostream& os, const Order& order);