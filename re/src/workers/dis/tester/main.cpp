#include <core/component.h>
#include <core/loggers/print_logger.h>

#include "dis_worker.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

void GotPdu(const KDIS::PDU::Header& pdu){
    std::cerr << "Got PDU: " << pdu.GetAsString() << std::endl;
}

int main(){
    Component c("test");
    Dis_Worker worker(c, "worker");

    Print::Logger::get_logger().SetLogLevel(10);
    worker.logger().AddLogger(Print::Logger::get_logger());

    worker.SetAttributeValue<std::string>("ip_address", "192.168.111.255");
    worker.SetAttributeValue<int>("port", 3000);
    
    worker.SetPduCallback([=](const KDIS::PDU::Header& pdu){GotPdu(pdu);});

    worker.Configure();
    std::this_thread::sleep_for(std::chrono::milliseconds(50000));
}