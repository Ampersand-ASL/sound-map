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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Looks for the first USB sound device that matches the parameters provided and 
 * returns the HID, ALSA, and OSS device names that correspond. Use caution
 * since these devices have a way of changing order during reboots or reconfigurations
 * so be specific as possible. Specification of physical bus/port number is ideal.
 * 
 * @param matchBusId The physical USB bus ID (a number) or null if any bus 
 * is acceptable.
 * @param matchPortId The physical USB port ID (a number) of null if any 
 * port is acceptable.
 * @param matchVendorId The vendor ID of the USB device (a 4 character hex
 * code) or null if any vendor is acceptable.
 * @param matchProductId The product ID of the USB device (a 4 character hex
 * code) or null if any product is acceptable.
 * @param hidDevice If not null, gets a string of the format "/dev/hidrawX"
 * where X is and integer.
 * @param alsaDevice If not null, gets a string of the format "X,0" where X
 * is the card number.
 * @param ossDevice If not null, gets a string of the format /dev/dsp or 
 * /dev/dspX where X is the device number >= 1.
 * @returns 0 on success, -10 if no matching device is found.
 */
int soundMap(
    const char* matchBusId, const char* matchPortId, 
    const char* matchVendorId, const char* matchProductId, 
    char* hidDevice, unsigned hidDeviceLen,
    char* alsaDevice, unsigned alsaDeviceLen,
    char* ossDevice, unsigned ossDeviceLen);

/**
 * Parses a simple query string and calls soundMap().
 *  
 * @param query Is of the format: "bus:aaa,port:bbb,vendor:ccc,product:ddd,vendorname:nnnn"
 * @returns 0 on success, -10 if no matching device is found, -20 if there 
 * is a format error in the query.
 */
int querySoundMap(
    const char* query,
    char* hidDevice, unsigned hidDeviceLen,
    char* alsaDevice, unsigned alsaDeviceLen,
    char* ossDevice, unsigned ossDeviceLen);

/**
 * A callback used for directory entry visitation.
 */
typedef void (*directoryVisitor)(const char* vendorId, const char* vendorName, 
    const char* productId, const char* productName, void*);

/**
 * Visits all entries in the USB directory.
 * Data credit: http://www.linux-usb.org/usb.ids
 */
void visitVendorProductDirectory(directoryVisitor cb, void* userData);    

/**
 * A utility function for getting the hex vendor ID from a human-readiable
 * vendor name.
 * 
 * @returns 0 on success, -10 when not found.
 */
int resolveVendorName(const char* targetName, char* vendorId, unsigned vendorIdLen);

/**
 * Converts vendor/product ID pair into the corresponding full name pair.
 * 
 * @param vendorName Will be filled with a null-terminated name, as much as
 *   will fit.
 * @param prodcutName Will be filled with a null-terminated name, as much as
 *   will fit.
 */
void getVendorAndProductName(const char* targetVendorId, const char* targetProductId, 
    char* vendorName, unsigned vendorNameLen, char* productName, unsigned productNameLen);

typedef void (*deviceVisitor)(const char* vendorId, const char* productId, 
    unsigned busId, unsigned portId, void* userData);

/**
 * Iterates across all USB devices and calls the callback for each one.
 * @param userData Will be passed back in the callback function.
 */
int visitUSBDevices(deviceVisitor cb, void* userData);

typedef void (*deviceVisitor2)(const char* vendorName, const char* productName, 
    const char* busId, const char* portId);

/**
 * Iterates across all USB devices and calls the callback for each one.
 * @param userData Will be passed back in the callback function.
 */
int visitUSBDevices2(deviceVisitor2 cb, void* userData);

#ifdef __cplusplus
}
#endif

