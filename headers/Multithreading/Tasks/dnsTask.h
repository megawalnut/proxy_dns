#ifndef DNSTASK_H
#define DNSTASK_H

#include "../../DNS/dnsDispatcher.h"
#include "../../DNS/dnsParser.h"
#include "../outQueue.h"
#include "task.h"

class DNSTask : public Task {
public:
    DNSTask(DNSDispatcher& disp, 
            OutPacket::Packet packet);
    ~DNSTask() = default;
    
    void execute(OutQueue& out, bool isOk) override;

private:
    DNSDispatcher& m_disp;
    OutPacket::Packet m_packet;
};

#endif // DNSTASK_H
