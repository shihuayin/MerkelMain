#include "VolumeGraph.h"
#include "OrderBook.h"
#include <iostream>
#include "OrderBookEntry.h"
#include "MerkelMain.h"
#include <vector>
#include <iomanip>
#include <sstream>

SumOfVolumeGraph::SumOfVolumeGraph(const std::string& timestamp, double volume)
    : timestamp(timestamp),
      volume(volume)
{
}

// Get a valid product input from the user
std::string SumOfVolumeGraph::candleProduct(OrderBook& orderB)
{
    while (true)
    {
        std::vector<std::string> acceptableProducts = orderB.getKnownProducts();
        std::string userInputProduct;
        std::cout << "Enter a product: eg ETH/BTC" << std::endl;
        std::getline(std::cin, userInputProduct);

        if (std::find(acceptableProducts.begin(), acceptableProducts.end(), userInputProduct) != acceptableProducts.end())
        {
            return userInputProduct;
        }
        else
        {
            std::cout << "Sorry, invalid product. Please try again.";
        }
    }
}

// Get a valid candle type input from user
std::string SumOfVolumeGraph::candleType()
{
    while (true)
    {
        std::vector<std::string> acceptableTypes{"ask", "bid"};
        std::string userInputType;
        std::cout << "Enter a type: ask or bid" << std::endl;
        std::getline(std::cin, userInputType);

        if (std::find(acceptableTypes.begin(), acceptableTypes.end(), userInputType) != acceptableTypes.end())
        {
            return userInputType;
        }
        else
        {
            std::cout << "Sorry, invalid type! Please try again.";
        }
    }
}

// Create vector 
std::vector<SumOfVolumeGraph> SumOfVolumeGraph::createSumOfVolumeGraph(OrderBook& orderB)
{
    std::string product = candleProduct(orderB);
    OrderBookType type = OrderBookEntry::stringToOrderBookType(candleType());
    std::vector<std::string> timestamp = {currentTime};

    const int numOfTime = 5;
    std::vector<SumOfVolumeGraph> volumeVector;

    // Loop to gather volume data for each timestamp
    for (int i = 0; i < numOfTime; i++)
    {
        std::vector<OrderBookEntry> currentOrders = orderB.getOrders(type, product, timestamp[i]);

        double sumOfVolume = 0.0;
        for (const auto& order : currentOrders)
        {
            sumOfVolume += order.amount;
        }

        SumOfVolumeGraph vg(timestamp[i], sumOfVolume);
        volumeVector.push_back(vg);
  
        if (i < numOfTime - 1) 
        {
            std::string nextTime = orderB.getNextTime(timestamp[i]);
            timestamp.push_back(nextTime);
        }
    }

    // Display the volumes on the console
    for (const auto& cans : volumeVector)
    {
        std::cout << "Timestamp     : " << cans.timestamp << "\n"
                  << "Sum of Volume : " << cans.volume << "\n"
                  << "------------------------------" << std::endl;
    }

    return volumeVector;
}

// Total trading 
std::vector<std::string> SumOfVolumeGraph::generateVolumeString(const SumOfVolumeGraph& SumOfVolumeGraph, int area, double max_volume, double min_volume)
{
    int sumOfVolumeStart = area - round((max_volume - SumOfVolumeGraph.volume) / ((max_volume - min_volume) / area));
    int sumOfVolumeEnd = round((0 - min_volume) / ((max_volume - min_volume) / area));

    std::vector<std::string> stringList(area, "");
    for (int m = area, n = 0; m > 0; m--, n++)
    {
        if (m > sumOfVolumeStart || m < sumOfVolumeEnd)
        {
            stringList[n] = "     ";
        }
        else
        {
            stringList[n] = "█████";
        }
    }

    return stringList;
}

// Plot the volume graph
void SumOfVolumeGraph::plotSumOfVolumeGraph(const std::vector<SumOfVolumeGraph>& data)
{

    double minVolume = 0.0;
    double maxVolume = std::numeric_limits<double>::min();
    int maxVolumeIndex = -1;
    const int area = 20;

    for (size_t i = 0; i < data.size(); i++)
    {
        if (data[i].volume > maxVolume)
        {
            maxVolume = data[i].volume;
            maxVolumeIndex = i;
        }
    }

    std::ostringstream maxVolumeStream;
    maxVolumeStream << std::fixed << std::setprecision(2) << maxVolume;
    std::string maxVolumeString = maxVolumeStream.str();
    int yAxisWidth = maxVolumeString.length();

    std::vector<std::string> finalStringList(area, "");
    std::vector<std::string> stringList(area, "");

    std::string yellow = "\033[1;33m";
    std::string reset = "\033[0m";


    // 1 print volume
    for (size_t i = 0; i < data.size(); i++)
    {
        std::vector<std::string> stringList = generateVolumeString(data[i], area, maxVolume, minVolume);
        for (int j = 0; j < area; j++)
        {
            // Highlight the max volume
            if (i == maxVolumeIndex && (stringList[j] == "█████" ))
            {
                finalStringList[j] += yellow + stringList[j] + reset + "         ";
            }
            else
            {
                finalStringList[j] += stringList[j] + "         ";
            }
        }
    }

  //2 y-axis
    for (int i = 0; i < area; i++)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << maxVolume - ((maxVolume - minVolume) * i / (area - 1));
        std::string aux = stream.str();
        std::cout << std::setw(yAxisWidth) << aux << "| " << finalStringList[i] << std::endl;
    }
    std::cout << std::setw(yAxisWidth) << " " << std::string(65, '-') << std::endl;

// 3 x-axis
    int j = yAxisWidth + 1; 
    int k = 8;
    for (size_t i = 0; i < data.size(); i++)
    {
        std::string timestamp = data[i].timestamp;
        std::string time = timestamp.substr(timestamp.find(" ") + 1, k);

        std::cout << std::string(j, ' ') << time;
        j = 6; 
    }
    std::cout << std::endl;
}





