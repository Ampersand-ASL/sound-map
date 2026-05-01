/**
 * USB Sound Device Mapping
 * Copyright (C) 2025, Bruce MacKinnon KC1FSZ
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>

#include <iostream>

#include <libusb-1.0/libusb.h>

#include "sound-map.h"
#include "usb-dir.h"

#define MAX_ALSA (16)
#define MAX_HID (16)

using namespace std;

namespace kc1fsz {

int makePortPath(libusb_device* dev, char* portPath, unsigned portPathCapacity) {
    // IMPORTANT: Because the path is limited to 7 levels we can be 
    // safe writing this into a 32-byte buffer.
    if (portPathCapacity < 32)
        return -1;
    uint8_t ports[7];
    int portsLen = libusb_get_port_numbers(dev, ports, std::size(ports));
    unsigned p = 0;
    portPath[p++] = '0' + libusb_get_bus_number(dev);
    portPath[p++] = '-';
    for (int k = 0; k < portsLen; k++) {
        if (k > 0) 
            portPath[p++] = '.';
        portPath[p++] = '0' + ports[k];
    }
    portPath[p++] = 0;
    return 0;
}

int visitUSBDevices(std::function<void(const char* vendorId, const char* productId, 
    const char* portPath, int usbBus, int usbDevice)> cb) {

    libusb_context *ctx = 0;
    if (libusb_init(&ctx) < 0) {
        return -1;
    }

    libusb_device** devList;
    ssize_t count = libusb_get_device_list(ctx, &devList);
    if (count < 0) {
        libusb_exit(ctx);
        return -2;
    }

    // Iterate across all of the USB devices on the machine.

    for (ssize_t i = 0; i < count; i++) {

        libusb_device *dev = devList[i];
        struct libusb_device_descriptor desc;

        if (libusb_get_device_descriptor(dev, &desc) != 0) 
            continue;
            
        char vendorId2[16];
        snprintf(vendorId2, 16, "%04x", desc.idVendor);
        char productId2[16];
        snprintf(productId2, 16, "%04x", desc.idProduct);

        // IMPORTANT: Because the path is limited to 7 levels we can be 
        // safe writing this into a 32-byte buffer.
        char portPath[32] = { 0 };
        makePortPath(dev, portPath, sizeof(portPath));

        // Fire the callback
        cb(vendorId2, productId2, portPath,
            libusb_get_bus_number(dev), libusb_get_device_address(dev));
    }
  
    // The "1" causes an unref
    libusb_free_device_list(devList, 1);

    // 5. Deinitialize libusb
    libusb_exit(ctx);

    return 0;
}

/**
 * Iterates across all USB devices and calls the callback for each one.
 * @param userData Will be passed back in the callback function.
 */
int visitUSBDevices2(std::function<void(const char* vendorName, const char* productName, 
    const char* vendorId, const char* productId, const char* portPath,
    int usbBus, int usbDevice)> cb) {
    return visitUSBDevices(
        [cb](const char* vendorId, const char* productId, const char* portPath, 
            int usbBus, int usbDevice) {

            string vendorName, productName;

            // Attempt to resolve the vendor ID/product ID pair into full names
            int rc = getVendorAndProductName(vendorId, productId, 
                vendorName, productName);
            // If the lookup fails then just use the IDs
            if (rc < 0) {
                vendorName = "vid ";
                vendorName += vendorId;
                productName = "pid ";
                productName += productId;
            }

            cb(vendorName.c_str(), productName.c_str(), vendorId, productId, portPath, 
                usbBus, usbDevice);
        }   
    );
}

int resolveUSBSoundDevice(const char* portPath, int& alsaCard, string& ossDevice) {
    
    // Check for the case where nothing is specified
    if ((portPath == 0 || portPath[0] == 0))
        return -10;

    // Visit all of the devices to find one that matches, then resolve into ALSO/OSS
    // device names.
    bool pathFound = false;
    bool deviceFound = false;

    int rc = visitUSBDevices([portPath, &pathFound, &deviceFound, &alsaCard, &ossDevice]
        (const char*, const char*, const char* portPath2, int usbBus2, int usbDevice2) {
            if (!pathFound) {
                if (strcmp(portPath, portPath2) == 0) {

                    pathFound = true;

                    // Just because we match on the USB IDs doesn't mean we found 
                    // a valid ALSA card. The next step establishes that by checking 
                    // each ALSA card to see if it matches on the bus#/port#.

                    // ALSA card search
                    int alsaCardFound = -1;
                    for (unsigned alsaCard = 0; alsaCard < MAX_ALSA; alsaCard++) {

                        // Look into the /proc/asound/card<alsa_card_number>/usbbus file
                        // and match on the string that looks like:
                        //
                        // 00b/00d 
                        //
                        // Where "b" is bus number and "d" is device number.

                        char alsaCardName[64];
                        snprintf(alsaCardName, 64, "/proc/asound/card%d/usbbus", alsaCard);
                        FILE* f0 = fopen(alsaCardName, "r");
                        if (f0 == 0)
                            continue;
                        char alsaNeedle[16];
                        snprintf(alsaNeedle, 16, "%03d/%03d", usbBus2, usbDevice2);
                        char contents[64];
                        contents[0] = 0;
                        fgets(contents, 63, f0);
                        if (strstr(contents, alsaNeedle) != 0) {
                            alsaCardFound = alsaCard;
                        }
                        fclose(f0);
                    }

                    if (alsaCardFound != -1) {

                        alsaCard = alsaCardFound;
                        deviceFound = true;

                        // At the moment we are assuming that the ALSA-OSS emulation layer
                        // is numbering the OSS DSP devices in the same order as the ALSA
                        // cards on the machine.
                        //
                        // This can be changed so this function should might need to be 
                        // improved if this case is common.
                        char temp[64];
                        if (alsaCardFound == 0) {
                            snprintf(temp, 64, "/dev/dsp");
                        } else {
                            snprintf(temp, 64, "/dev/dsp%d", alsaCardFound);
                        }
                        ossDevice = temp;
                    }
                }
            }
        }
    );

    if (rc != 0)
        return -1;
    else if (!pathFound)
        return -2;
    else if (!deviceFound)
        return -3;
    else 
        return 0;
}

int resolveUSBHIDDevice(const char* portPath, string& hidDevice) {

    // Check for the case where nothing is specified
    if ((portPath == 0 || portPath[0] == 0))
        return -10;

    // Visit all of the devices to find one that matches, then resolve into ALSO/OSS
    // device names.
    bool pathFound = false;
    bool deviceFound = false;

    int rc = visitUSBDevices([portPath, &pathFound, &deviceFound, &hidDevice]
        (const char*, const char*, const char* portPath2, int usbBus2, int usbDevice2) {
            if (!pathFound) {
                if (strcmp(portPath, portPath2) == 0) {

                    pathFound = true;

                    char hidNeedle[64];
                    snprintf(hidNeedle, sizeof(hidNeedle), "/usb%d/%d-%c/%s/", 
                        usbBus2, usbBus2,
                        // Top-level port number
                        portPath2[2],
                        // Full path
                        portPath2);

                    for (unsigned hid = 0; hid < MAX_HID; hid++) {
                        char hidDev[64];
                        snprintf(hidDev, 64, "/sys/class/hidraw/hidraw%d", hid);
                        char hidLinkTarget[1024];
                        ssize_t len = readlink(hidDev, hidLinkTarget, sizeof(hidLinkTarget) - 1);
                        if (len != -1) {
                            hidLinkTarget[len] = '\0';
                            if (strstr(hidLinkTarget, hidNeedle) != 0) {
                                char temp[64];
                                snprintf(temp, 64, "/dev/hidraw%d", hid);
                                hidDevice = temp;
                                deviceFound = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    );

    if (rc != 0)
        return -1;
    else if (!pathFound)
        return -2;
    else if (!deviceFound)
        return -3;
    else 
        return 0;
}

}
