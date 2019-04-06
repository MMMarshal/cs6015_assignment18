//
//  utilities.hpp
//  assignment16
//
//  Created by Marshal Foster on 4/6/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#ifndef utilities_hpp
#define utilities_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <math.h>
#include "event.hpp"

void printQ(std::priority_queue<Event, std::vector<Event>, EventCompare> q);
void printTotalServiceDurations(const std::vector<double> *totalserviceDurations);
double calculatePercentile(std::vector<double> totalserviceDurations, const double percentile);
std::vector<double> calculatePercentiles(std::vector<double> totalserviceDurations, const std::vector<int> percentiles);
void printResults(enum Scenario scenario, const std::vector<int> percentiles, const std::vector<double> percentileResults);

#endif /* utilities_hpp */
