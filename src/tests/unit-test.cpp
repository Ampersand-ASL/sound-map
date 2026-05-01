#include <iostream>
#include <string>
#include <cassert>

#include "sound-map.h"
#include "serial-map.h"

using namespace std;
using namespace kc1fsz;

int test_1() {

    int alsaDev;
    string ossDev;

    int rc = resolveUSBSoundDevice("3-2.2", alsaDev, ossDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    string hidDev;

    rc = resolveUSBHIDDevice("1-3.2", hidDev);
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
    int rc = resolveUSBSoundDevice("1-3.2", alsaDev, ossDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    string hidDev;
    rc = resolveUSBHIDDevice("1-3.2", hidDev);
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
           const char* portPath, int usbBus, int usbDevice) {
            cout << "vendorName   " << vendorName << endl;
            cout << "productName  " << productName << endl;
            cout << "vendorId     " << vendorId << endl;
            cout << "productId    " << productId << endl;
            cout << "portPath     " << portPath << endl;
            cout << "bus          " << usbBus << endl;
            cout << "device       " << usbDevice << endl;
        }
    );
    assert(rc == 0);
    return 0;
}    

int serial_0() {
    cout << endl << "Serial devices:" << endl;
    int rc = visitUSBSerialDevices(
        [](const char* dev, const char* portPath) {
            cout << "dev          " << dev << endl;
            cout << "portPath     " << portPath << endl;
        }
    );
    assert(rc == 0);
    return 0;
}    

int serial_1() {
    string dev;
    assert(resolveUSBSerialDevice("1-2", dev) == 0);
    cout << "Query result: " << dev << endl;
    return 0;
}

int main(int, const char**) {
    test_1();
    test_2();
    test_5();
    serial_0();
    serial_1();
}
