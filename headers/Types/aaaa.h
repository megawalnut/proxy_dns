#ifndef AAAA_H
#define AAAA_H

#include "iType.h"

class AAAA final : public IType {
public:
    void handle(const ldns_rr* rr) const override;
};

#endif // AAAA_H