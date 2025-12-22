#include <stdio.h>
#include <iostream>
#include "sound-map.h"

using namespace std;

void visitor(const char* vendorId2, const char* productId2, 
    unsigned busId, unsigned portId, void* userData) {

    printf("bus  %d\n", busId);
    printf("port %d\n", portId);
    printf("vendor %s\n", vendorId2);
    printf("product %s\n", productId2);
}

int main(int, const char**) {
    //visitUSBDevices(visitor, 0);
    char vname[16], pname[16];
    getVendorAndProductName("0d8c", "0001", vname, 16, pname, 16);
    cout << vname << "/" << pname << endl;

    char vid[5];
    assert(resolveVendorName("C-Media Electronics, Inc.", vid, 5) == 0);
    cout << "VID " << vid << endl;
    assert(resolveVendorName("C-Media Electonics, Inc.", vid, 5) != 0);
}

