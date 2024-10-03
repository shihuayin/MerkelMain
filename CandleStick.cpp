#include "CandleStick.h"
#include "OrderBook.h"
#include <iostream>
#include "OrderBookEntry.h"
#include "MerkelMain.h"
#include <vector>
#include <iomanip>
#include <sstream>

CandleStick::CandleStick(const std::string& timestamp,
                         double open_price,
                         double highest_price,
                         double lowest_price,
                         double close_price)
:   timestamp(timestamp),
    open_price(open_price),
    highest_price(highest_price),
    lowest_price(lowest_price),
    close_price(close_price)
{
}

// handle product
std::string CandleStick::candleProduct(OrderBook& orderB) {
    std::vector<std::string> acceptableProducts = orderB.getKnownProducts();
    std::string userInputProduct;

    do {
        std::cout << "Please enter a product (e.g., ETH/BTC): ";
        std::getline(std::cin, userInputProduct);

        if (std::find(acceptableProducts.begin(), acceptableProducts.end(), userInputProduct) != acceptableProducts.end()) {
            return userInputProduct;
        }

        std::cout << "Invalid product! Please try again." << std::endl;
    } while (true);
}

// handle type 
std::string CandleStick::candleType()
{

    std::vector<std::string> acceptableTypes{"ask", "bid"};
        std::string userInputType;
    do{
        std::cout << "Please enter a type: ask or bid" << std::endl;
        std::getline(std::cin, userInputType);

        if(std::find(acceptableTypes.begin(), acceptableTypes.end(), userInputType) != acceptableTypes.end())
        {
            return userInputType;
        }
            std::cout << "Invalid type! Please try again.";
    }while (true); 
}


// Generate a vector of CandleStick objects based on user input
std::vector<CandleStick> CandleStick::generateCandlestick(OrderBook& orderB)
{

    std::vector<std::string> timestamp = {currentTime};
    std::vector<CandleStick> candleVector;
    OrderBookType input_type = OrderBookEntry::stringToOrderBookType(CandleStick::candleType());
    std::string input_product = CandleStick::candleProduct(orderB);
    
    if (previousTime.empty()){
        previousTime = orderB.getLastTime();
    }
    std::vector<OrderBookEntry> previousOrders = orderB.getOrders(input_type, input_product, previousTime);
    
    
//  5 times
const int numOfTime=5;
    for (int i = 0; i < numOfTime; i++)
        {
            std::vector<OrderBookEntry> currentOrders = orderB.getOrders(input_type, input_product, timestamp[i]);
            
    // Calculate close price
            double closePrice;

            if (!currentOrders.empty()){
       double sumOfCloseValue{0.0}, sumOfCloseAmount{0.0};

        for (const auto& order : currentOrders)
        {
            sumOfCloseValue += order.amount * order.price;
            sumOfCloseAmount += order.amount;
        }
        closePrice = sumOfCloseValue / sumOfCloseAmount; 
    }
    else
    {
        closePrice = 0; 
    }

 // Calculate open price
    double openPrice;
    if (!previousOrders.empty())
    {
        double sumOfOpenValue{0.0}, sumOfOpenAmount{0.0};

        for (const auto& order : previousOrders)
        {
            sumOfOpenValue += order.price * order.amount;
            sumOfOpenAmount += order.amount;
        }
        openPrice = sumOfOpenValue / sumOfOpenAmount; 
    }
    else if (!currentOrders.empty())
    {
        openPrice = currentOrders[0].price; 
    }
    else
    {
        openPrice = 0; 
    }

    // Calculate high and low price
    double highestPrice = OrderBook::getHighPrice(currentOrders);
    double lowestPrice = OrderBook::getLowPrice(currentOrders);


    CandleStick cans(timestamp[i], openPrice, highestPrice, lowestPrice, closePrice);
    candleVector.push_back(cans);

    previousOrders = currentOrders;
    if (i < 4)
    {
        std::string nextTime = orderB.getNextTime(timestamp[i]);
        timestamp.push_back(nextTime);
    }
}
// Print the created CandleStick objects
    for (const auto& cans : candleVector)
    {

         std::cout << std::fixed << std::setprecision(8);
        std::cout << "Timestamp     : " << cans.timestamp << "\n"
                  << "Open Price    : " << cans.open_price << "\n"
                  << "Highest Price : " << cans.highest_price << "\n"
                  << "Lowest Price  : " << cans.lowest_price << "\n"
                  << "Close Price   : " << cans.close_price << "\n"
                  << "————————————————————————————————————————————" << std::endl;
    }
    return candleVector;
}



// Generate ascii graph
std::vector<std::string> CandleStick::generateCandlestickString(const CandleStick& candlestick, int area, double max, double min) {
    
    double priceScale = (max - min) / area;
    int candleS = area - round((max - candlestick.highest_price) / priceScale);
    int candleE = round((candlestick.lowest_price - min) / priceScale);

    // Determine the color of the candlestick
    bool isRed = candlestick.open_price > candlestick.close_price;
    int lineS = area - round((max - (isRed ? candlestick.close_price : candlestick.open_price)) / priceScale);
    int lineE = area - round((max - (isRed ? candlestick.open_price : candlestick.close_price)) / priceScale);


    const std::string redColor = "\033[31m";   // red
    const std::string greenColor = "\033[32m"; //  green
    const std::string resetColor = "\033[0m";  

    std::vector<std::string> stringList(area, "");

    // Iterate through each column of the candlestick row
    for (int i = area, j = 0; i > 0; i--, j++) {
        if (i > candleS || i < candleE) {
            stringList[j] = "     ";
        } else {
            // Determine the color based on the candlestick being red or green
            std::string color = isRed ? redColor : greenColor; 
            // Check if the current position is within the line range of the candlestick    
            if ((i > lineS && i < lineE) || (i < lineS && i > lineE)||(i == lineS || i == lineE)) {
                stringList[j] = color + "█████" + resetColor;
            } else {
            stringList[j] = "  " + color + "│" + resetColor + "  ";
                }
        }
    }
    return stringList;
}

// Print the candlestick graph
void CandleStick::printCandleStick(const std::vector<CandleStick>& data) {

    double minPrice = std::numeric_limits<double>::max();
    double maxPrice = std::numeric_limits<double>::min();
   
    for (const auto& candlestick : data) {
        if (candlestick.lowest_price < minPrice) minPrice = candlestick.lowest_price;
        if (candlestick.highest_price > maxPrice) maxPrice = candlestick.highest_price;
    }
    const int area = 20;

    std::vector<std::string> finalStringList(area, "");
    std::vector<std::string> stringList(area, "");
    for (const auto& candlestick : data) {
        stringList = generateCandlestickString(candlestick, area, maxPrice, minPrice);
        for (int i = 0; i < area; i++) {
            finalStringList[i] += stringList[i] + "         ";
        }
    }

    double verticalChange = (maxPrice - minPrice) / area;
    double yMax = maxPrice - verticalChange/2;
    double yMin = minPrice + verticalChange/2;

// Print each row of the candlestick graph along with y-axis values
for (int i = 0; i < area; i++) {
    double lineValue = yMax - ((yMax - yMin) * i / (area - 1));

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8) << lineValue;
    std::string aux = oss.str();
    while (aux.length() < 10) {
        aux = " " + aux;
    }
    std::cout << aux << "| ";
    std::cout << finalStringList[i] << std::endl;
}

    std::cout << std::string(8, ' ') << std::string(65, '-') << std::endl;

    // Print x-axis
    std::cout << "          ";
    for (size_t i = 0; i < data.size(); i++) {
        std::string timestamp = data[i].timestamp;
        std::string time = timestamp.substr(timestamp.find(" ") + 1, 8);
        std::cout << std::setw(8) << time;

        // Add spacing between timestamps 
        std::cout << "      ";

    }
    std::cout << std::endl;
}





