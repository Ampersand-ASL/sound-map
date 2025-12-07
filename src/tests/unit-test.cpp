#include <iostream>
#include "sound-map.h"

using namespace std;

/*
This example looks on bus 3, port 2
*/
int test_1() {

    char hidDev[32];
    char alsaDev[32];
    char ossDev[32];
    int rc = soundMap("3", "2", 0, 0, hidDev, 32, alsaDev, 32, ossDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    return 0;
}

/*
This example looks on bus 3, port 2
*/
int test_2() {
    
    char hidDev[32];
    char alsaDev[32];
    char ossDev[32];
    int rc = querySoundMap("bus=3,port=2", hidDev, 32, alsaDev, 32, ossDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    return 0;
}

/*
This example does a query on bus 3 for vendor code 0d8c.
*/
int test_3() {
    
    char hidDev[32];
    char alsaDev[32];
    char ossDev[32];
    int rc = querySoundMap("bus=3,vendor=0d8C", hidDev, 32, alsaDev, 32, ossDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found" << endl;
    cout << "HID   : " << hidDev << endl;
    cout << "ALSA  : " << alsaDev << endl;
    cout << "OSS   : " << ossDev << endl;

    return 0;
}

int main(int, const char**) {
    test_1();
    test_2();
    test_3();
}

