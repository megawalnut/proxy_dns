#ifndef CNAME_H
#define CNAME_H

#include "iType.h"

class CNAME final : public IType {
public:
    void handle(const ldns_rr* rr) const override;
};

#endif // CNAME_H