#ifndef NS_H
#define NS_H

#include "iType.h"

class NS final : public IType {
public:
    void handle(const ldns_rr* rr) const override;
};

#endif // NS_H