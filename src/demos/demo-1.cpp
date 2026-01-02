#include <iostream>
#include "sound-map.h"

using namespace std;
using namespace kc1fsz;

int main(int, const char**) {
    
    const char* targetVendorName = "C-Media Electronics, Inc.";
    char query[64];
    snprintf(query, 64, "vendorname:\"%s\"", targetVendorName);

    string alsaDev;
    string ossDev;

    int rc = querySoundMap(query, alsaDev, ossDev);
    if (rc < 0) {
        cout << "ERROR: " << rc << endl;
        return -1;
    }

    cout << "Found the device:" << endl;
    cout << " ALSA  : " << alsaDev << endl;
    cout << " OSS   : " << ossDev << endl;

    return 0;
}
