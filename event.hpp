//
//  event.hpp
//  assignment16
//
//  Created by Marshal Foster on 4/6/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#ifndef event_hpp
#define event_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include "customer.hpp"

enum Scenario {
  BANK,
  SUPERMARKET
};

enum EventType {
  ARRIVE,
  FINISH
};

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

struct EventCompare {
  bool operator()(const Event& lhs, const Event& rhs) const {
    return lhs.getTimeStamp() > rhs.getTimeStamp();
  }
};

#endif /* event_hpp */
