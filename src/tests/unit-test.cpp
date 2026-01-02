#include <iostream>
#include <string>

#include "sound-map.h"

using namespace std;
using namespace kc1fsz;

/*
This example looks on bus 3, port 2
*/
int test_1() {

    string alsaDev;
    string ossDev;

    int rc = soundMap("1", "2", 0, 0, alsaDev, ossDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    string hidDev;

    rc = hidMap("1", "2", 0, 0, hidDev);
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
    
    string alsaDev;
    string ossDev;
    int rc = querySoundMap("bus:1,port:2", alsaDev, ossDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    string hidDev;
    rc = queryHidMap("bus:1,port:2", hidDev);
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
    
    string alsaDev;
    string ossDev;
    int rc = querySoundMap("bus:1,vendor:0d8C", alsaDev, ossDev);
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
    
    string hidDev;
    int rc = queryHidMap("vendorname:\"C-Media Electronics, Inc.\"", hidDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;

    return 0;
}

int test_5() {
    int rc = visitUSBDevices2(
        [](const char* vendorName, const char* productName, 
           const char* busId, const char* portId) {
            cout << "vendorName   "  << vendorName << endl;
            cout << "productName  " << productName << endl;
            cout << "busId        " << busId << endl;
            cout << "portId       " << portId << endl;
        }
    );
    return 0;
}    

int main(int, const char**) {
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
}
