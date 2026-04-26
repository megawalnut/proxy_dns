#ifndef ITYPE_H
#define ITYPE_H

#include <ldns/ldns.h>

class IType {
public:
    virtual ~IType() = default;

    //method for a specific type
    virtual void handle(const ldns_rr* rr) const = 0;
};

#endif // ITYPE_H
