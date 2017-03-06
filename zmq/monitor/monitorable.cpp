/* re_common
 * Copyright (C) 2016-2017 The University of Adelaide
 *
 * This file is part of "re_common"
 *
 * "re_common" is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * "re_common" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include "monitorable.h"
#include "monitor.h"

//Declare our Static variables
int zmq::Monitorable::count_ = 0;
std::mutex zmq::Monitorable::mutex_;

std::string zmq::Monitorable::GetNewMonitorAddress(){
    std::string addr = "inproc://Monitor_" + std::to_string(GetNewMonitorID());
    return addr;
}

int zmq::Monitorable::GetNewMonitorID(){
    std::unique_lock<std::mutex> lock(zmq::Monitorable::mutex_);
    return ++count_;
}