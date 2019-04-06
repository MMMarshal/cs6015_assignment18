//
//  event.cpp
//  assignment16
//
//  Created by Marshal Foster on 4/6/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#include "event.hpp"

Event::Event(int timeStamp, Customer customer, enum EventType type)
: customer(customer.getId(), customer.getArrivalTime(), customer.getServiceDuration(), customer.getProjectedFinishTime()) {
  this->timeStamp = timeStamp;
  this->type = type;
}

std::string Event::toString() const{
  std::string enumString = "";
  if (type == ARRIVE)
    enumString = "ARRIVE";
  else if (type == FINISH)
    enumString = "FINISH";
  
  return "TimeStamp: " + std::to_string(timeStamp) +
  ", Type: " + enumString +
  ", Id: " + std::to_string(customer.getId()) +
  ", Arrival: " + std::to_string(customer.getArrivalTime()) +
  ", Duration: " + std::to_string(customer.getServiceDuration()) +
  ", Finish: " + std::to_string(customer.getProjectedFinishTime());
}
