#ifndef DNSTASK_H
#define DNSTASK_H

#include "../../DNS/dnsDispatcher.h"
#include "../../DNS/dnsParser.h"
#include "../../DNS/packet.h"
#include "../outQueue.h"
#include "task.h"

class DNSTask : public Task {
public:
    DNSTask(DNSDispatcher& disp, 
            Packet packet);
    ~DNSTask() = default;
    
    void execute(OutQueue& out, bool isOk) override;

private:
    DNSDispatcher& m_disp;
    Packet m_packet;
};

#endif // DNSTASK_H
