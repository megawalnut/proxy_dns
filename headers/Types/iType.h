#ifndef ITYPE_H
#define ITYPE_H

#include <ldns/ldns.h>

#include "../utils.h"

class IType {
public:
    virtual ~IType() = default;

    //method for a specific type
    virtual bool handle(const ldns_rr* rr) const = 0;
};

#endif // ITYPE_H
