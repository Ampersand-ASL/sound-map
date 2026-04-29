#include <iostream>
#include <string>
#include <cassert>

#include "sound-map.h"

using namespace std;
using namespace kc1fsz;

int test_1() {

    int alsaDev;
    string ossDev;

    int rc = soundMap("1-3.2", 0, 0, alsaDev, ossDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    string hidDev;

    rc = hidMap("1-3.2", 0, 0, hidDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;

    return 0;
}

int test_2() {
    
    int alsaDev;
    string ossDev;
    int rc = querySoundMap("port:1-3.2", alsaDev, ossDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    string hidDev;
    rc = queryHidMap("port:1-3.2", hidDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;

    return 0;
}

int test_3() {
    
    int alsaDev;
    string ossDev;
    int rc = querySoundMap("port:1-3.2,vendor:0d8C", alsaDev, ossDev);
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
           const char* vendorId, const char* productId, 
           const char* portPath) {
            cout << "vendorName   " << vendorName << endl;
            cout << "productName  " << productName << endl;
            cout << "vendorId     " << vendorId << endl;
            cout << "productId    " << productId << endl;
            cout << "portPath     " << portPath << endl;
        }
    );
    assert(rc == 0);
    return 0;
}    

int parse_1() {
    string portPath, vendorId, productId;
    int rc = parseSoundMapQuery("port:1-1,vendor:ccc,product:ddd", 
        portPath, vendorId, productId);
    assert(rc == 0);
    assert(portPath == "1-1");
    assert(productId == "ddd");
    return 0;
}

int main(int, const char**) {
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    parse_1();
}
