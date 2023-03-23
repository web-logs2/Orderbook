#include "../header/Orderbook.hpp"

int main() {
    std::string input;
    Orderbook orderbook;
    while (getline(std::cin, input)) {
        std::stringstream ss(input);
        std::string command; ss >> command;
        if (command == "END") break;
        else if (command == "CXL") {
            std::string orderId; ss >> orderId;
            orderbook.CancelOrder(orderId);
        }
        else if (command == "SUB") {
            std::string type, side, orderId, quantity;
            ss >> type >> side >> orderId >> quantity;
            Side s = side == "B" ? Side::B : Side::S;
            if (type == MarketOrder) {
                Order order(s, orderId, std::stoi(quantity), 0);
                orderbook.InsertMarketOrder(order);
            }
            else if (type == LimitOrder) {
                std::string price; ss >> price;
                Order order(s, orderId, stoi(quantity), std::stoi(price));
                orderbook.InsertLimitOrder(order);
            }
        }
    } 
    orderbook.PrintOrderbook();
}