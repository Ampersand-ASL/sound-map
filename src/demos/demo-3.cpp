#include <stdio.h>

#include <iostream>
#include <cassert>
#include <cstring>

#include "sound-map.h"

using namespace std;

void visitor(const char* vId, const char* pId, 
    unsigned busId, unsigned portId, void* userData) {

    char vendorName[48];
    char productName[48];
    int rc = getVendorAndProductName(vId, pId, vendorName, 48, productName, 48);
    if (rc != 0) {
        strcpy(vendorName, "UNKNOWN");
        strcpy(productName, "UNKNOWN");
    }

    printf("bus  %d\n", busId);
    printf("port %d\n", portId);
    printf("vendor  %s %s\n", vId, vendorName);
    printf("product %s %s\n", pId, productName);
}

int main(int, const char**) {
    
    visitUSBDevices(visitor, 0);

    char vname[16], pname[16];
    getVendorAndProductName("0d8c", "0001", vname, 16, pname, 16);
    cout << vname << "/" << pname << endl;

    char vid[5];
    assert(resolveVendorName("C-Media Electronics, Inc.", vid, 5) == 0);
    cout << "VID " << vid << endl;
    assert(resolveVendorName("C-Media Electonics, Inc.", vid, 5) != 0);
}

