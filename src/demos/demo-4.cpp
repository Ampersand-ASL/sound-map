#include <dirent.h>
#include <stdio.h>
#include <string.h> // Required for strcmp

#include <iostream>
#include <vector>
#include <string>

#include "serial-map.h"

using namespace std;
using namespace kc1fsz;

int main(int, const char**) {

    visitUSBSerialDevices([](const char* dev,unsigned busId, unsigned portId) {
        cout << "dev " << dev << endl;
        cout << "  bus " << busId << " port " << portId << endl;
    });

    return 0;
}
