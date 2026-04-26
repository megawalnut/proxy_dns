#ifndef MX_H
#define MX_H

#include "iType.h"

class MX final : public IType {
public:
    void handle(const ldns_rr* rr) const override;
};

#endif // MX_H