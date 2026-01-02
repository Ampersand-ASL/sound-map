#include <iostream>
#include "sound-map.h"

using namespace std;

/*
This example looks on bus 3, port 2
*/
int test_1() {

    char alsaDev[32];
    char ossDev[32];

    int rc = soundMap("1", "2", 0, 0, alsaDev, 32, ossDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    char hidDev[32];

    rc = hidMap("1", "2", 0, 0, hidDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;

    return 0;
}

/*
This example looks on bus 1, port 2
*/
int test_2() {
    
    char alsaDev[32];
    char ossDev[32];
    int rc = querySoundMap("bus:1,port:2", alsaDev, 32, ossDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    char hidDev[32];
    rc = queryHidMap("bus:1,port:2", hidDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;

    return 0;
}

/*
This example does a query on bus 3 for vendor code 0d8c.
*/
int test_3() {
    
    char alsaDev[32];
    char ossDev[32];
    int rc = querySoundMap("bus:1,vendor:0d8C", alsaDev, 32, ossDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    return 0;
}

int test_4() {
    
    char hidDev[32];
    int rc = queryHidMap("vendorname:\"C-Media Electronics, Inc.\"", hidDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;

    return 0;
}

void v2(const char* vendorName, const char* productName, 
    const char* busId, const char* portId, void* userData) {
    cout << "vendorName   "  << vendorName << endl;
    cout << "productName  " << productName << endl;
    cout << "busId        " << busId << endl;
    cout << "portId       " << portId << endl;
}

int test_5() {
    int rc = visitUSBDevices2(v2, 0);
    return 0;
}    

int main(int, const char**) {
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
}
