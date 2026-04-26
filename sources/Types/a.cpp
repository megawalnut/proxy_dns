#include "../../headers/Types/a.h"

#include <iostream>
#include <string>

void A::handle(const ldns_rr* rr) const {
    const ldns_rdf* domain = ldns_rr_owner(rr);

    char* c_name = ldns_rdf2str(domain);
    if (!c_name) {
        return;
    }

    std::string name(c_name);
    std::cout << "DNS name: " << name << std::endl;

    free(c_name);
}