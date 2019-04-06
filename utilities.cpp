//
//  utilities.cpp
//  assignment16
//
//  Created by Marshal Foster on 4/6/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#include "utilities.hpp"

void printQ(std::priority_queue<Event, std::vector<Event>, EventCompare> q) {
  while (!q.empty()) {
    std::cout << q.top() << std::endl;;
    q.pop();
  }
  std::cout << std::endl;;
}

void printTotalServiceDurations(const std::vector<double> *totalserviceDurations) {
  std::cout << "Total Service Durations: ";
  for (int i = 0; i < totalserviceDurations->size(); ++i) {
    std::cout << totalserviceDurations->at(i) << ", ";
  }
  std::cout << std::endl;
}

double calculatePercentile(std::vector<double> totalserviceDurations, const double percentile) {
  assert(percentile > 0 && percentile < 1);
  //find percentile index
  int percentileIndex = round(totalserviceDurations.size() * percentile);
  //sort vector
  std::sort(totalserviceDurations.begin(), totalserviceDurations.end());
  //return
  return totalserviceDurations.at(percentileIndex);
}

std::vector<double> calculatePercentiles(std::vector<double> totalserviceDurations, const std::vector<int> percentiles) {
  std::vector<double> percentileResults;
  for (int i = 0; i < percentiles.size(); ++i) {
    double percentileValue = (double) percentiles.at(i) / 100;
    double percentile = calculatePercentile(totalserviceDurations, percentileValue);
    percentileResults.push_back(percentile);
  }
  return percentileResults;
}

void printResults(enum Scenario scenario, const std::vector<int> percentiles, const std::vector<double> percentileResults) {
  std::string title = "";
  if (scenario == BANK)
    title = "Bank service times in minutes: ";
  else
    title = "Supermarket service times in minutes: ";
  
  std::cout << title;
  for (int i = 0; i < percentiles.size(); ++i) {
    std::cout << percentiles.at(i) << "th %ile " << percentileResults.at(i);
    if (i != percentiles.size() - 1)
      std::cout << ", ";
  }
  std::cout << std::endl;
}

