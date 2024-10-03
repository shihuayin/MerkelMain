/*
This PDF contains a total of four files. 
Among them, CandleStick.h and CandleStick.cpp have completed 
the tasks for Task 1 and Task 2, 
while VolumeGraph.h and VolumeGraph.cpp have accomplished the task for Task 3
*/




#include <string>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include <vector>

class CandleStick
{
    public:
        CandleStick(const std::string& timestamp,
                    double open_price,
                    double close_price,
                    double highest_price,
                    double lowest_price);
        static std::string candleProduct(OrderBook& orderB);
        static std::string candleType();
        static std::vector<CandleStick> generateCandlestick(OrderBook& orderB);
        static void printCandleStick(const std::vector<CandleStick>& data);
        static std::vector<std::string> generateCandlestickString(const CandleStick& candleStick, int area, double max, double min);

    private:
            std::string timestamp;
            double open_price;
            double close_price;
            double highest_price;
            double lowest_price;
            

};


