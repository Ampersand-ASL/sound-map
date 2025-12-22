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
    visitUSBDevices(visitor, 0);
}

