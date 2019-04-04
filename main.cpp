//
//  main.cpp
//  assignment16
//
//  Created by Adam Quintana on 3/31/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <string>
#include <math.h>
#include <tuple>
#include <algorithm>

int const SIMULATION_TIME_IN_SECONDS = 43200;
//int const SIMULATION_TIME_IN_SECONDS = 5000;
int const NUMBER_OF_CASHIERS = 6;

//global clock
int runningClock;

enum Scenario {
    BANK,
    SUPERMARKET
};

enum EventType {
    ARRIVE,
    FINISH
};

class Customer {
public:
    Customer(int id, int arrivalTime, int serviceDuration, int projectedFinishTime);
    int getId() const { return this->id; }
    int getArrivalTime() const { return this->arrivalTime; }
    int getServiceDuration() const { return this->serviceDuration; }
    int getProjectedFinishTime() const { return this->projectedFinishTime; }
private:
    int id;
    int arrivalTime;
    int serviceDuration;
    int projectedFinishTime;
};

Customer::Customer(int id, int arrivalTime, int serviceDuration, int projectedFinishTime) {
    this->id = id;
    this->arrivalTime = arrivalTime;
    this->serviceDuration = serviceDuration;
    this->projectedFinishTime = projectedFinishTime;
}

class Event {
public:
    Event(int timeStamp, Customer customer, enum EventType type);
    friend std::ostream& operator <<(std::ostream& stream, const Event& event);
    std::string toString() const;
    int getTimeStamp() const { return timeStamp; }
    Customer getCustomer() const { return this->customer; }
    enum EventType getType() const { return this->type; }
private:
    int timeStamp;
    Customer customer;
    enum EventType type;
};

Event::Event(int timeStamp, Customer customer, enum EventType type)
: customer(customer.getId(), customer.getArrivalTime(), customer.getServiceDuration(), customer.getProjectedFinishTime()) {
    this->timeStamp = timeStamp;
    this->type = type;
}

std::string Event::toString() const{
    std::string enumString = "";
    if (type == ARRIVE) {
        enumString = "ARRIVE";
    } else if (type == FINISH) {
        enumString = "FINISH";
    }
    return
        "TimeStamp: " + std::to_string(timeStamp) +
        ", Type: " + enumString +
        ", Id: " + std::to_string(customer.getId()) +
        ", Arrival: " + std::to_string(customer.getArrivalTime()) +
        ", Duration: " + std::to_string(customer.getServiceDuration()) +
        ", Finish: " + std::to_string(customer.getProjectedFinishTime());
}

std::ostream& operator <<(std::ostream& stream, const Event& event)
{
    stream << event.toString();
    return stream;
}

struct EventCompare
{
    bool operator()(const Event& lhs, const Event& rhs) const
    {
        if (lhs.getTimeStamp() < rhs.getTimeStamp()) {
            return false;
        }
        return true;
    }
};

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

std::tuple<int, int> findMinimumProjectedFinishAndIndex(std::vector<std::deque<Customer>>* queues) {
    int minQueueIndex = -1;
    int minQueueProjectedFinish = INT_MAX;
    
    //loop through all queues
    for (int i = 0; i < queues->size(); ++i) {
        auto queue = queues->at(i);
        
        //if queue is empty return current queue @ current time
        if (queue.size() == 0) {
            return std::make_tuple(runningClock, i);
        }
        
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

Event arrive(Scenario scenario, std::vector<std::deque<Customer>> *queues, Customer *customer) {
    if (scenario == SUPERMARKET) {
        
        //find minimum projected finish and queue index
        std::tuple<int, int> minQueueTuple = findMinimumProjectedFinishAndIndex(queues);
        int minQueueProjectedFinishTime = std::get<0>(minQueueTuple);
        int minQueueIndex = std::get<1>(minQueueTuple);
        assert(minQueueProjectedFinishTime >= runningClock);
        assert(minQueueIndex >= 0 && minQueueIndex <= NUMBER_OF_CASHIERS);
        
        //add customer to queue @ queue's projected finish time + customer's service duration
        int newFinishTime = minQueueProjectedFinishTime + customer->getServiceDuration();
        Customer updatedCustomer = Customer(customer->getId(), customer->getArrivalTime(), customer->getServiceDuration(), newFinishTime);
        queues->at(minQueueIndex).push_back(updatedCustomer);
        
        //create and returnevent for projected finish time
        return Event(newFinishTime, updatedCustomer, FINISH);
        
    } else { //BANK
        
        //find minimum projected finish and queue index
        std::tuple<int, int> minQueueTuple = findMinimumProjectedFinishAndIndex(queues);
        int minQueueProjectedFinishTime = std::get<0>(minQueueTuple);
        int minQueueIndex = std::get<1>(minQueueTuple);
        assert(minQueueProjectedFinishTime >= runningClock);
        assert(minQueueIndex >= 0 && minQueueIndex <= NUMBER_OF_CASHIERS);
        
        //add customer to queue @ queue's projected finish time + customer's service duration
        int newFinishTime = minQueueProjectedFinishTime + customer->getServiceDuration();
        Customer updatedCustomer = Customer(customer->getId(), customer->getArrivalTime(), customer->getServiceDuration(), newFinishTime);
        
        //
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
            if (firstCustomer.getId() == customer->getId()) {
                return i;
            }
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
    if (scenario == BANK) {
        title = "Bank service times in minutes: ";
    } else {
        title = "Supermarket service times in minutes: ";
    }
    std::cout << title;
    for (int i = 0; i < percentiles.size(); ++i) {
        std::cout << percentiles.at(i) << "th %ile " << percentileResults.at(i);
        if (i != percentiles.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

std::vector<double> runSimulation(enum Scenario scenario, std::priority_queue<Event, std::vector<Event>, EventCompare> eventQ) {
    
    //initialize global clock
    runningClock = 0;
    
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
            Event newEvent = arrive(scenario, queues, &customer);
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
    if (argc != 4) {
        std::cout << "invalid args" << std::endl;
    }
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
