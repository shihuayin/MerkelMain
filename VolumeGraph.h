#include <string>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include <vector>

class SumOfVolumeGraph
{
public:
    SumOfVolumeGraph(const std::string& timestamp,
                double volume);
    static std::string candleProduct(OrderBook& orderB);
    static std::string candleType();
    static std::vector<SumOfVolumeGraph> createSumOfVolumeGraph(OrderBook& orderB);
    static void plotSumOfVolumeGraph(const std::vector<SumOfVolumeGraph>& data);
    static std::vector<std::string> generateVolumeString(const SumOfVolumeGraph& SumOfVolumeGraph, int area, double max_volume, double min_volume);
private:
    std::string timestamp;
    double volume;
};


