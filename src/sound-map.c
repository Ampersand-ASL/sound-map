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

#include <libusb-1.0/libusb.h>

#include "sound-map.h"
#include "usb-dir.h"

static const unsigned MAX_ALSA = 16;
static const unsigned MAX_HID = 16;

int visitUSBDevices(deviceVisitor cb, void* userData) {

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

        if (cb) {
            cb(vendorId2, productId2, 
                libusb_get_bus_number(dev), libusb_get_port_number(dev), userData);
        }
    }
  
    // The "1" causes an unref
    libusb_free_device_list(devList, 1);

    // 5. Deinitialize libusb
    libusb_exit(ctx);

    return 0;
}

struct visitArgs {
    deviceVisitor2 cb;
    void* userData;
};

static void visitor1(const char* vendorId, const char* productId, 
    unsigned busId, unsigned portId, void* userData) {
    struct visitArgs* args = (struct visitArgs*)userData;
    char vendorName[48];
    vendorName[0] = 0;
    char productName[48];
    productName[0] = 0;

    // Attempt to resolve the vendor ID/product ID pair into full names
    int rc = getVendorAndProductName(vendorId, productId, 
        vendorName, 48, productName, 48);
    // If the lookup fails then just use the IDs
    if (rc < 0) {
        snprintf(vendorName, 48, "vid %s", vendorId);
        snprintf(productName, 48, "pid %s", productId);
    }

    char busIdStr[8];
    char portIdStr[8];
    snprintf(busIdStr, 8, "%u", busId);
    snprintf(portIdStr, 8, "%u", portId);

    args->cb(vendorName, productName, busIdStr, portIdStr, args->userData);
}

/**
 * Iterates across all USB devices and calls the callback for each one.
 * @param userData Will be passed back in the callback function.
 */
int visitUSBDevices2(deviceVisitor2 cb, void* userData) {
    struct visitArgs args;
    args.cb = cb;
    args.userData = userData;
    return visitUSBDevices(visitor1, &args);
}

int soundMap(
    const char* busId, const char* portId, const char* vendorId, const char* productId, 
    char* alsaDevice, unsigned alsaDeviceLen,
    char* ossDevice, unsigned ossDeviceLen) {

    // Check for the case where nothing is specified
    if ((busId == 0 || busId[0] == 0) &&
        (portId == 0 || portId[0] == 0) &&
        (vendorId == 0 || vendorId[0] == 0) &&
        (productId == 0 || productId[0] == 0))
        return -10;

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

    int found = 0;

    for (ssize_t i = 0; i < count; i++) {

        libusb_device *dev = devList[i];
        struct libusb_device_descriptor desc;

        if (libusb_get_device_descriptor(dev, &desc) != 0) 
            continue;
            
        char vendorId2[16];
        snprintf(vendorId2, 16, "%04x", desc.idVendor);
        char productId2[16];
        snprintf(productId2, 16, "%04x", desc.idProduct);
        
        if ((busId == 0 || busId[0] == 0 || libusb_get_bus_number(dev) == atoi(busId)) &&
            (portId == 0 || portId[0] == 0 || libusb_get_port_number(dev) == atoi(portId)) &&
            (vendorId == 0 || vendorId[0] == 0 || strcasecmp(vendorId, vendorId2) == 0) &&
            (productId == 0 || productId[0] == 0 || strcasecmp(productId, productId2) == 0)) {

            found = 1;

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
                // is numbering the OSS DSP devices in the same order as the ALSA
                // cards on the machine.
                //
                // This can be changed so this function should might need to be 
                // improved if this case is common.
                if (alsaCardFound != -1) {
                    if (alsaCardFound == 0) {
                        snprintf(ossDevice, ossDeviceLen, "/dev/dsp");
                    } else {
                        snprintf(ossDevice, ossDeviceLen, "/dev/dsp%d", alsaCardFound);
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

int hidMap(
    const char* busId, const char* portId, const char* vendorId, const char* productId, 
    char* hidDevice, unsigned hidDeviceLen) {

    // Check for the case where nothing is specified
    if ((busId == 0 || busId[0] == 0) &&
        (portId == 0 || portId[0] == 0) &&
        (vendorId == 0 || vendorId[0] == 0) &&
        (productId == 0 || productId[0] == 0))
        return -10;

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

    int found = 0;

    for (ssize_t i = 0; i < count; i++) {

        libusb_device *dev = devList[i];
        struct libusb_device_descriptor desc;

        if (libusb_get_device_descriptor(dev, &desc) != 0) 
            continue;
            
        char vendorId2[16];
        snprintf(vendorId2, 16, "%04x", desc.idVendor);
        char productId2[16];
        snprintf(productId2, 16, "%04x", desc.idProduct);
        
        if ((busId == 0 || busId[0] == 0 || libusb_get_bus_number(dev) == atoi(busId)) &&
            (portId == 0 || portId[0] == 0 || libusb_get_port_number(dev) == atoi(portId)) &&
            (vendorId == 0 || vendorId[0] == 0 || strcasecmp(vendorId, vendorId2) == 0) &&
            (productId == 0 || productId[0] == 0 || strcasecmp(productId, productId2) == 0)) {

            found = 1;

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

// An internal function that converts a query into the parameters needed
// by the map functions.
static int parseQuery(const char* query, 
    char* busId, unsigned busIdLen,
    char* portId, unsigned portIdLen,
    char* vendorId, unsigned vendorIdLen,
    char* productId, unsigned productIdLen) {

    busId[0] = 0;
    portId[0] = 0;
    vendorId[0] = 0;
    productId[0] = 0;
    char vendorName[64];
    vendorName[0] = 0;

    // A name/value pair parser. Format is n:v,n:v, ...
    char mode = 'n';
    int inQuote = 0;
    const unsigned nameCap = 32;
    char name[nameCap];
    name[0] = 0;
    const unsigned valueCap = 32;
    char value[valueCap];
    value[0] = 0;
    unsigned nPtr = 0;
    unsigned vPtr = 0;
    for (unsigned i = 0; i <= strlen(query); i++) {
        if (inQuote) {
            // Not allowed to end the query in a quote
            if (query[0] == 0) {
                return -15;
            } 
            // Look for the end of the quote. This is the only way to get out.
            else if (query[i] == '"') {
                inQuote = 0;
            }
            // Still quoted
            else {
                value[vPtr++] = query[i];
                value[vPtr] = 0;
            }
        }
        else {
            if (query[i] == ',' || query[i] == 0) {
                if (strcmp(name, "bus") == 0) 
                    strcpy(busId, value);
                else if (strcmp(name, "port") == 0) 
                    strcpy(portId, value);
                else if (strcmp(name, "vendor") == 0) 
                    strcpy(vendorId, value);
                else if (strcmp(name, "product") == 0) 
                    strcpy(vendorId, value);
                else if (strcmp(name, "vendorname") == 0) 
                    strcpy(vendorName, value);
                else 
                    return -20;
                // Get ready for the next pair
                mode = 'n';
                nPtr = 0;
                name[0] = 0;
                vPtr = 0;
                value[0] = 0;
            }
            else if (query[i] == ' ') {
                // Ignore
            }
            else if (query[i] == ':') {
                // Switch modes
                mode = 'v';
            } 
            else {
                if (mode == 'n') {
                    if (nPtr < nameCap - 1) {
                        name[nPtr++] = query[i];
                        name[nPtr] = 0;
                    }
                } else if (mode == 'v') {
                    // This is the only way to trigger a quote
                    if (query[i] == '"') {
                        inQuote = 1;
                    } else if (vPtr < valueCap - 1) {
                        value[vPtr++] = query[i];
                        value[vPtr] = 0;
                    }
                } else assert(0);
            }
        }
    }

    // If a name is specified use it to determine the vendor ID
    if (vendorName[0] != 0) {
        int rc = resolveVendorName(vendorName, vendorId, 32);
        if (rc < 0)
            return rc;
    }

    return 0;
}

/**
 * This function just parses the query string and calls the search function.
 */
int querySoundMap(
    const char* query,
    char* alsaDevice, unsigned alsaDeviceLen,
    char* ossDevice, unsigned ossDeviceLen) {

    char busId[32];
    busId[0] = 0;
    char portId[32];
    portId[0] = 0;
    char vendorId[32];
    vendorId[0] = 0;
    char productId[32];
    productId[0] = 0;

    int rc = parseQuery(query, 
        busId, 32,
        portId, 32,
        vendorId, 32,
        productId, 32);
    if (rc < 0)
        return rc;

    return soundMap(busId, portId, vendorId, productId, 
        alsaDevice, alsaDeviceLen,
        ossDevice, ossDeviceLen);
}

/**
 * This function just parses the query string and calls the search function.
 */
int queryHidMap(
    const char* query,
    char* hidDevice, unsigned hidDeviceLen) {

    char busId[32];
    busId[0] = 0;
    char portId[32];
    portId[0] = 0;
    char vendorId[32];
    vendorId[0] = 0;
    char productId[32];
    productId[0] = 0;

    int rc = parseQuery(query, 
        busId, 32,
        portId, 32,
        vendorId, 32,
        productId, 32);
    if (rc < 0)
        return rc;

    return hidMap(busId, portId, vendorId, productId, 
        hidDevice, hidDeviceLen);
}



