/**
 * USB Device Mapping
 * Copyright (C) 2026, Bruce MacKinnon KC1FSZ
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <string>

#include "serial-map.h"

using namespace std;

namespace kc1fsz {

int querySerialDevices(const char* query, std::string& ttyDevice) {
    bool found = false;
    // Traverse the USB serial devices
    visitUSBSerialDevices(
        [query, &ttyDevice, &found](const char* dev, unsigned busId, unsigned portId) {
            if (!found) {
                // Make the value
                char value[32];
                snprintf(value, sizeof(value), "usb bus:%u,port:%u", busId, portId);
                if (strcmp(query, value) == 0) {
                    ttyDevice = dev;
                    found = true;
                }
            }
        }
    );
    return found ? 0 : -1;
}

int visitUSBSerialDevices(std::function<void(const char* dev, unsigned busId, unsigned portId)> cb) {

    vector<string> interfaces;

    DIR *d;
    struct dirent *dir;

    d = opendir("/sys/bus/usb/devices");
    if (d) {
        // Read each entry in the directory
        while ((dir = readdir(d)) != NULL) {
            // Skip the special entries for current ('.') and parent ('..') directories
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) 
                continue;
            // Look for the interfaces
            if (strchr(dir->d_name, ':') != 0)
                interfaces.push_back(string(dir->d_name));
        }
        // Close the directory stream
        closedir(d);
    }

    // Go through each interface and see if it has a ttyUSBx 
    for (string& interface : interfaces) {
        string p = string("/sys/bus/usb/devices") + "/" + interface;

        d = opendir(p.c_str());
        if (d) {
            // Read each entry in the directory
            while ((dir = readdir(d)) != NULL) {
                string dn = dir->d_name;
                // Skip the special entries for current ('.') and parent ('..') directories
                if (dn == "." || dn == "..") 
                    continue;
                if (dn.starts_with("ttyUSB")) {
                    string dev = "/dev/";
                    dev += dn;
                    // Parse the bus/port
                    int state = 0;
                    char acc[16];
                    unsigned accPtr = 0;
                    unsigned bus = 0, port = 0;
                    for (unsigned i = 0; i < interface.length(); i++) {
                        if (state == 0) {
                            if (interface[i] == '-') {
                                acc[accPtr] = 0;
                                accPtr = 0;
                                bus = atoi(acc);
                                state = 1;
                            }
                            else {
                                acc[accPtr] = interface[i];
                                if (accPtr < sizeof(acc) - 1)
                                    accPtr++;
                            }
                        }
                        else if (state == 1) {
                            if (interface[i] == ':') {
                                acc[accPtr] = 0;
                                accPtr = 0;
                                port = atoi(acc);
                                state = 2;
                            }
                            else {
                                acc[accPtr] = interface[i];
                                if (accPtr < sizeof(acc) - 1)
                                    accPtr++;
                            }
                        }                       
                    }
                    cb(dev.c_str(), bus, port);
                }
            }
            // Close the directory stream
            closedir(d);
        }
    }

    return 0;
}

}
