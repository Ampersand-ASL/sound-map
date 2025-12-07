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
#include <string.h>
#include <unistd.h> // For readlink

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <libusb-1.0/libusb.h>

#include "sound-map.h"

using namespace std;

namespace kc1fsz {

static const unsigned MAX_ALSA = 16;
static const unsigned MAX_HID = 16;

int soundMap(
    const char* busId, const char* portId, const char* vendorId, const char* productId, 
    char* hidDevice, unsigned hidDeviceLen,
    char* alsaDevice, unsigned alsaDeviceLen,
    char* ossDevice, unsigned ossDeviceLen) {

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

    // Iterate across all of the USB devices on the machine and return 
    // the first one that satisfies the query parameters.

    bool found = false;

    for (ssize_t i = 0; i < count; i++) {

        libusb_device *dev = devList[i];
        struct libusb_device_descriptor desc;

        if (libusb_get_device_descriptor(dev, &desc) != 0) 
            continue;
            
        char vendorId2[16];
        snprintf(vendorId2, 16, "%04x", desc.idVendor);
        char productId2[16];
        snprintf(productId2, 16, "%04x", desc.idProduct);
        
        if ((busId == 0 || libusb_get_bus_number(dev) == atoi(busId)) &&
            (portId == 0 || libusb_get_port_number(dev) == atoi(portId)) &&
            (vendorId == 0 || strcmp(vendorId, vendorId2) == 0) &&
            (productId == 0 || strcmp(productId, productId2) == 0)) {

            found = true;

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
                snprintf(alsaNeedle, 16, "%03d/%03d", 
                    libusb_get_bus_number(dev), libusb_get_device_address(dev));
                char contents[64];
                contents[0] = 0;
                fgets(contents, 63, f0);
                if (strstr(contents, alsaNeedle) != 0) {
                    alsaCardFound = alsaCard;
                }
                fclose(f0);
            }

            if (alsaDeviceLen > 0) {
                alsaDevice[0] = 0;
                if (alsaCardFound != -1)
                    snprintf(alsaDevice, alsaDeviceLen, "%d,%d", alsaCardFound, 0);
            }
        
            if (ossDeviceLen > 0) {
                ossDevice[0] = 0;
                // At the moment we are assuming that the ALSA-OSS emulation layer
                // is numbering the OSS DSP devices in the same order as the ALAS
                // cards on the machine.
                if (alsaCardFound != -1) {
                    if (alsaCardFound == 0) {
                        snprintf(ossDevice, ossDeviceLen, "/dev/dsp");
                    } else {
                        snprintf(ossDevice, ossDeviceLen, "/dev/dsp%d", alsaCardFound);
                    }
                }
            }

            // HID search

            if (hidDeviceLen > 0) {
                hidDevice[0] = 0;

                char hidNeedle[32];
                snprintf(hidNeedle, 32, "/usb%d/%d-%d/", 
                    libusb_get_bus_number(dev), 
                    libusb_get_bus_number(dev), 
                    libusb_get_port_number(dev));

                for (unsigned hid = 0; hid < MAX_HID; hid++) {
                    char hidDev[64];
                    snprintf(hidDev, 64, "/sys/class/hidraw/hidraw%d", hid);
                    char hidLinkTarget[1024];
                    ssize_t len = readlink(hidDev, hidLinkTarget, sizeof(hidLinkTarget) - 1);
                    if (len != -1) {
                        hidLinkTarget[len] = '\0';
                        if (strstr(hidLinkTarget, hidNeedle) != 0) {
                            snprintf(hidDevice, hidDeviceLen, "/dev/hidraw%d", hid);
                            break;
                        }
                    }
                }
            }
            
            break;
        }
    }
  
    // The "1" causes an unref
    libusb_free_device_list(devList, 1);

    // 5. Deinitialize libusb
    libusb_exit(ctx);

    if (!found) 
        return -10;

    return 0;
}
}

