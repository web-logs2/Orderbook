#include "../header/Order.hpp"

std::ostream& operator<<(std::ostream& os, const Order& order) {
    std::cout << order.getQuantity() << "@" << order.getPrice() << "#" << order.getOrderId();
    return os;
}