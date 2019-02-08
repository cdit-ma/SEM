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
    auto c = std::make_shared<Component>("Test");
    Dis_Worker worker(*c, "worker");

    Print::Logger::get_logger().SetLogLevel(10);
    worker.logger().AddLogger(Print::Logger::get_logger());

    auto ip_address_attr = worker.GetAttribute("ip_address").lock();
    auto port_attr = worker.GetAttribute("port").lock();
    worker.SetPduCallback([=](const KDIS::PDU::Header& pdu){GotPdu(pdu);});

    //Multicast bois
    ip_address_attr->set_String("192.168.111.255");
    port_attr->set_Integer(3000);

    worker.Configure();
    std::this_thread::sleep_for(std::chrono::milliseconds(50000));
}