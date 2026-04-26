#ifndef TXT_H
#define TXT_H

#include "iType.h"

class TXT final : public IType {
public:
    void handle(const ldns_rr* rr) const override;
};

#endif // TXT_H