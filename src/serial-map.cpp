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

int resolveUSBSerialDevice(const char* portPath, std::string& ttyDevice) {

    bool pathFound = false;

    // Traverse the USB serial devices
    int rc = visitUSBSerialDevices(
        [portPath, &ttyDevice, &pathFound](const char* dev, const char* portPath2) {
            if (!pathFound) {
                if (strcmp(portPath, portPath2) == 0) {
                    ttyDevice = dev;
                    pathFound = true;
                }
            }
        }
    );

    if (rc < 0)
        return -1;
    else if (!pathFound)
        return -2;
    else 
        return 0;
}

static void parsePort(const string& interface, char* acc, unsigned capacity) {
    // Parse the port path
    int state = 0;
    unsigned accPtr = 0;
    for (unsigned i = 0; i < interface.length(); i++) {
        if (state == 0) {
            if (interface[i] == ':') {
                acc[accPtr] = 0;
                state = 1;
            }
            else {
                acc[accPtr] = interface[i];
                if (accPtr < capacity - 1)
                    accPtr++;
            }
        }
    }
}

static void visitDir(const string& base, std::function<void(const string& name)> cb) {
    DIR *d;
    struct dirent *dir;
    d = opendir(base.c_str());
    if (d) {
        // Read each entry in the directory
        while ((dir = readdir(d)) != NULL) {
            // Skip the special entries for current ('.') and parent ('..') directories
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) 
                continue;
            cb(string(dir->d_name));
        }
        // Close the directory stream
        closedir(d);
    }
}

int visitUSBSerialDevices(std::function<void(const char* dev, const char* portPath)> cb) {

    // Build a list of candidate interfaces. Not all of these will have valid USB devices.
    vector<string> interfaces;
    visitDir("/sys/bus/usb/devices", [&interfaces](const string& name) {
        // Look for the interfaces (bus-port.port.port:i.i)
        if (name.contains(":"))
            interfaces.push_back(name);
    });

    // Go through each candidate interface and see if it has either a ttyUSBx or a tty/ttyACMx
    for (string& interface : interfaces) {

        char portName[16];
        parsePort(interface, portName, sizeof(portName));
        bool found = false;

        visitDir(string("/sys/bus/usb/devices") + "/" + interface, 
            [&cb, &portName, &interface, &found](const string& name) {
            if (!found) {
                // Look for the USB case (FTTI, Prolific chips, etc.)
                if (name.starts_with("ttyUSB")) {
                    string dev = "/dev/" + name;
                    cb(dev.c_str(), portName);
                    found = true;
                }
                // Look for the ACM case. ACM0 generally handles USB CDC (Communication Device 
                // Class) devices (often native USB/modems)
                else if (name == "tty") {
                    // Check down one more level
                    visitDir(string("/sys/bus/usb/devices") + "/" + interface + "/tty", 
                        [&cb, &portName, &found](const string& name2) {
                            if (name2.starts_with("ttyACM")) {
                                string dev2 = "/dev/" + name2;
                                cb(dev2.c_str(), portName);
                                found = true;
                            }
                        }
                    );
                }
            }
        });
    }
    return 0;
}

}
