#include <iostream>
#include "sound-map.h"

using namespace std;

int main(int, const char**) {
    
    char hidDev[32];
    char alsaDev[32];
    char ossDev[32];

    int rc = querySoundMap("bus:3,port:2", hidDev, 32, alsaDev, 32, ossDev, 32);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found the device:" << endl;
    cout << " HID   : " << hidDev << endl;
    cout << " ALSA  : " << alsaDev << endl;
    cout << " OSS   : " << ossDev << endl;

    return 0;
}
