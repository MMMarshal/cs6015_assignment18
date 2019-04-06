//
//  main.cpp
//  assignment16
//
//  Created by Adam Quintana on 3/31/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#include <random>
#include "customer.hpp"
#include "event.hpp"
#include "utilities.hpp"

int const SIMULATION_TIME_IN_SECONDS = 43200;
int const NUMBER_OF_CASHIERS = 6;

std::ostream& operator <<(std::ostream& stream, const Event& event) {
    stream << event.toString();
    return stream;
}

std::priority_queue<Event, std::vector<Event>, EventCompare>* fillQueue(double customersPerMinute, double maxServiceDurationInMinutes, int seed) {
    
    //initialize random number generator
    int maxServiceDurationInSeconds = round(maxServiceDurationInMinutes * 60);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<> serviceDurationDistribution(0, maxServiceDurationInSeconds);
    
    //fill queue
    double customersPerSecond = customersPerMinute / 60;
    int secondsPerCustomer = ceil(1 / customersPerSecond);
    auto q = new std::priority_queue<Event, std::vector<Event>, EventCompare>();
    int id = 0;
    for (int numSeconds = secondsPerCustomer;
         numSeconds < SIMULATION_TIME_IN_SECONDS;
         numSeconds += secondsPerCustomer) {
        int serviceDuration = serviceDurationDistribution(rng);
        Customer customer = Customer(id, numSeconds, serviceDuration, -1);
        Event event = Event(numSeconds, customer, ARRIVE);
        q->push(event);
        id++;
    }
    return q;
}

std::tuple<int, int> findMinimumProjectedFinishAndIndex(std::vector<std::deque<Customer>>* queues, int runningClock) {
    int minQueueIndex = -1;
    int minQueueProjectedFinish = INT_MAX;
    
    //loop through all queues
    for (int i = 0; i < queues->size(); ++i) {
        auto queue = queues->at(i);
        //if queue is empty return current queue @ current time
        if (queue.size() == 0)
            return std::make_tuple(runningClock, i);
        //last customer holds projected finish for each queue
        Customer lastCustomer = queue.back();
        int queueProjectedFinishTime = lastCustomer.getProjectedFinishTime();
        if (queueProjectedFinishTime < minQueueProjectedFinish) {
            minQueueProjectedFinish = queueProjectedFinishTime;
            minQueueIndex = i;
        }
    }
    return std::make_tuple(minQueueProjectedFinish, minQueueIndex);
}

Event arrive(Scenario scenario, std::vector<std::deque<Customer>> *queues, Customer *customer, int runningClock) {
    //find minimum projected finish and queue index
    std::tuple<int, int> minQueueTuple = findMinimumProjectedFinishAndIndex(queues, runningClock);
    int minQueueProjectedFinishTime = std::get<0>(minQueueTuple);
    int minQueueIndex = std::get<1>(minQueueTuple);
    assert(minQueueProjectedFinishTime >= runningClock);
    assert(minQueueIndex >= 0 && minQueueIndex <= NUMBER_OF_CASHIERS);
  
    if (scenario == SUPERMARKET) {
        //add customer to queue @ queue's projected finish time + customer's service duration
        int newFinishTime = minQueueProjectedFinishTime + customer->getServiceDuration();
        Customer updatedCustomer = Customer(customer->getId(), customer->getArrivalTime(), customer->getServiceDuration(), newFinishTime);
        queues->at(minQueueIndex).push_back(updatedCustomer);

        //create and returnevent for projected finish time
        return Event(newFinishTime, updatedCustomer, FINISH);
        
    } else { //BANK
        //add customer to queue @ queue's projected finish time + customer's service duration
        int newFinishTime = minQueueProjectedFinishTime + customer->getServiceDuration();
        Customer updatedCustomer = Customer(customer->getId(), customer->getArrivalTime(), customer->getServiceDuration(), newFinishTime);
      
        if (minQueueProjectedFinishTime == runningClock) { //if queue is empty, add customer
            queues->at(minQueueIndex).push_back(updatedCustomer);
            return Event(newFinishTime, updatedCustomer, FINISH);
        } else { //if queue is full, create ARRIVE event to add back to event queue
            return Event(newFinishTime, updatedCustomer, ARRIVE);
        }
    }
}

int findQueueIndex(const std::vector<std::deque<Customer>> *queues, const Customer *customer) {
    for (int i = 0; i < queues->size(); ++i) {
        auto queue = queues->at(i);
        if (queue.size() > 0) {
            Customer firstCustomer = queue.front();
            if (firstCustomer.getId() == customer->getId())
                return i;
        }
    }
    //should never get here
    assert(false);
    return -1;
}

void finish(Scenario scenario, std::vector<std::deque<Customer>> *queues, const Customer *customer) {
     if (scenario == SUPERMARKET) {
         int queueIndex = findQueueIndex(queues, customer);
         auto finishedCustomer = queues->at(queueIndex).front();
         assert(finishedCustomer.getId() == customer->getId());
         queues->at(queueIndex).pop_front();
     } else { //BANK
         int queueIndex = findQueueIndex(queues, customer);
         auto finishedCustomer = queues->at(queueIndex).front();
         assert(finishedCustomer.getId() == customer->getId());
         queues->at(queueIndex).pop_front();
     }
}

int findTotalNumberOfCustomers(std::priority_queue<Event, std::vector<Event>, EventCompare> q) {
    int count = 0;
    while (!q.empty()) {
        Event event = q.top();
        if (event.getType() == ARRIVE) {
            count++;
        }
        q.pop();
    }
    return count;
}

std::vector<double> runSimulation(enum Scenario scenario, std::priority_queue<Event, std::vector<Event>, EventCompare> eventQ) {
    //initialize global clock
    int runningClock = 0;
    //initialize cashier queues
    std::vector<std::deque<Customer>>* queues = new std::vector<std::deque<Customer>>(NUMBER_OF_CASHIERS);
    //initialize results vector
    int totalNumberOfCustomers = findTotalNumberOfCustomers(eventQ);
    std::vector<double> totalServiceDurations(totalNumberOfCustomers);
    
    while (!eventQ.empty()) {
        //printQ(eventQ);
        Event event = eventQ.top();
        eventQ.pop();
        Customer customer = event.getCustomer();
        runningClock = event.getTimeStamp();
        if (event.getType() == ARRIVE) {
            Event newEvent = arrive(scenario, queues, &customer, runningClock);
            eventQ.push(newEvent);
        } else if (event.getType() == FINISH) {
            finish(scenario, queues, &customer);
            assert(customer.getProjectedFinishTime() >= customer.getArrivalTime());
            totalServiceDurations.at(customer.getId()) = (double)(customer.getProjectedFinishTime() - customer.getArrivalTime()) / 60;
            //printTotalServiceDurations(&totalServiceDurations);
        }
    }
    std::vector<int> percentiles(3);
    percentiles.at(0) = 10;
    percentiles.at(1) = 50;
    percentiles.at(2) = 90;
    auto percentileResults = calculatePercentiles(totalServiceDurations, percentiles);
    printResults(scenario, percentiles, percentileResults);
    return percentileResults;
}

int main(int argc, const char * argv[]) {
    //validate command line arguements
    if (argc != 4)
        std::cout << "invalid args" << std::endl;
  
    double customersPerMinute = std::stod(argv[1]);
    assert(customersPerMinute >= 0);
    double maxServiceTime = std::stod(argv[2]);
    assert(maxServiceTime >= 0);
    int seed = std::stoi(argv[3]);
    assert(seed >= 0);
    
    //fill customer queue
    auto eventQ = fillQueue(customersPerMinute, maxServiceTime, seed);
    
    //bank simulation
    auto bankPercentileResults = runSimulation(BANK, *eventQ);
    
    //supermarket simulation
    auto supermarketPercentileResults = runSimulation(SUPERMARKET, *eventQ);
    
    return 0;
}
