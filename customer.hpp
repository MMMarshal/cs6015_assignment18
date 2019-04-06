//
//  customer.hpp
//  assignment16
//
//  Created by Marshal Foster on 4/6/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#ifndef customer_hpp
#define customer_hpp

#include <stdio.h>

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

#endif /* customer_hpp */
