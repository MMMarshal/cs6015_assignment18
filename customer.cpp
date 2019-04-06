//
//  customer.cpp
//  assignment16
//
//  Created by Marshal Foster on 4/6/19.
//  Copyright © 2019 Adam Quintana. All rights reserved.
//

#include "customer.hpp"

Customer::Customer(int id, int arrivalTime, int serviceDuration, int projectedFinishTime) {
  this->id = id;
  this->arrivalTime = arrivalTime;
  this->serviceDuration = serviceDuration;
  this->projectedFinishTime = projectedFinishTime;
}
