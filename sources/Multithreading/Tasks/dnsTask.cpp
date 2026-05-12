#include "../../../headers/Multithreading/Tasks/dnsTask.h"

DNSTask::DNSTask(DNSDispatcher& disp, 
                 Packet packet) 
                 : 
                 m_disp(disp), 
                 m_packet(packet) 
                 {}

void DNSTask::execute(OutQueue& out, bool isOk) {
    std::cout << "Receive packet size: " << m_packet.size << std::endl;
    isOk = false;
    // parsing qPacket
    const auto& [okDes, qPacket] = DNSParser::deserialize(m_packet.data, m_packet.size);
    if(okDes != Parse::Status::Ok) {
        std::cerr << "DNSServer::run: Deserialize failed" << std::endl;
        return;
    }

    // get aPacket
    DNSParser::DNSPtr aPacket = m_disp.dispatch(qPacket);
    if(!aPacket) {
        std::cerr << "DNSServer::run: Dispatcher packet empty" << std::endl;
        return;
    }

    // parsing aPacket
    const auto& [okSer, answer] = DNSParser::serialize(aPacket);
    if(okSer != Parse::Status::Ok) {
        std::cerr << "DNSServer::run: Serialize failed" << std::endl;
        return;
    }
    
    Packet packet = m_packet;
    packet.data = answer;
    packet.size = answer.size();
    isOk = true;
    out.push(packet);
}