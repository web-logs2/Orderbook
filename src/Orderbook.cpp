#include "../header/Orderbook.hpp"

template<typename T1, typename T2>
void Orderbook::deductQuantity(T1& orderbook, T2& orders, int value) {
    orderbook.begin()->deductQuantity(value);
    if (!orderbook.begin()->getQuantity()) {
        orderbook.erase(orderbook.begin());
        orders.erase(orderbook.begin()->getOrderId());
    }
}

template<typename T1, typename T2>
int Orderbook::matchMarketOrders(T1& orderbook, T2& orders, Order& order) {
    int totalTraded = 0;
    while (order.getQuantity() && !orderbook.empty()) {
        int quantityTraded = std::min(order.getQuantity(), orderbook.begin()->getQuantity());
        int price = orderbook.begin()->getPrice();
        deductQuantity<T1, T2>(orderbook, orders, quantityTraded);
        order.deductQuantity(quantityTraded);
        totalTraded += price * quantityTraded;
    }
    return totalTraded;
}

template<typename T1, typename T2>
void Orderbook::cancelOrderInOrderMap(std::string orderId, T1& orderbook, T2& ordermap) {
    orderbook.erase(ordermap[orderId]);
    ordermap.erase(orderId);
}

template<typename T1, typename T2>
void Orderbook::insertToOrderbook(T1& orderbook, T2& orders, Order& order) {
    orderbook.insert(order);
    orders[order.getOrderId()] = orderbook.find(order);
}

void Orderbook::InsertLimitOrder(Order& order) {
        // inserting the order into respective books
    if (order.isBuy()) insertToOrderbook<BuyOrderbook, BuyOrderMap>(buyOrderbook, buyOrderMap, order);
    else insertToOrderbook<SellOrderbook, SellOrderMap>(sellOrderbook, sellOrderMap, order);
    int totalTraded = 0;
    // match the orders if possible
    while (!buyOrderbook.empty() && !sellOrderbook.empty() 
        && buyOrderbook.begin()->getPrice() >= sellOrderbook.begin()->getPrice()) {
            auto buyOrder = buyOrderbook.begin();
            auto sellOrder = sellOrderbook.begin();
            int quantityTraded = std::min(buyOrder->getQuantity(), sellOrder->getQuantity());
            int priceTraded = buyOrder->getPrice();
            // remove from orderbook if quantity is depleted
            deductQuantity<BuyOrderbook, BuyOrderMap>(buyOrderbook, buyOrderMap, quantityTraded);
            deductQuantity<SellOrderbook, SellOrderMap>(sellOrderbook, sellOrderMap, quantityTraded);
            totalTraded += quantityTraded * priceTraded;
        }   
    std::cout << totalTraded << "\n";
}

void Orderbook::InsertMarketOrder(Order& order) {
    //int totalTraded = 0;
    if (order.isBuy()) std::cout << matchMarketOrders<SellOrderbook, SellOrderMap>(sellOrderbook, sellOrderMap, order) << "\n";
    else std::cout << matchMarketOrders<BuyOrderbook, BuyOrderMap>(buyOrderbook, buyOrderMap, order) << "\n";
}

void Orderbook::CancelOrder(std::string orderId) {
    if (buyOrderMap.find(orderId) != buyOrderMap.end()) cancelOrderInOrderMap<BuyOrderbook, BuyOrderMap>(orderId, this->buyOrderbook, this->buyOrderMap);     
    if (sellOrderMap.find(orderId) != sellOrderMap.end()) cancelOrderInOrderMap<SellOrderbook, SellOrderMap>(orderId, this->sellOrderbook, this->sellOrderMap);
}

void Orderbook::PrintOrderbook() {
    std::cout << "B: ";
    for (auto &order : buyOrderbook) std::cout << order << " ";
    std::cout << "\nS: ";
    for (auto &order : sellOrderbook) std::cout << order << " ";
    std::cout << "\n";
}