#ifndef A_H
#define A_H

#include "iType.h"

class A final : public IType {
public:
    void handle(const ldns_rr* rr) const override;
};

#endif // A_H