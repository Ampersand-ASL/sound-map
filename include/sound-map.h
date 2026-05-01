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

#include <functional>
#include <string>

namespace kc1fsz {

/**
 * Takes the path of a physical USB device and produces the ALSA/OSS device identifies.
 * This is tricky because the mapping depends on the USB enumeration process which
 * can change depending on conditions.
 * 
 * @param matchPortPath The physical USB port path
 * @param alsaCard Gets the card number.
 * @param ossDevice Gets a string of the format /dev/dsp or /dev/dspX where X is the 
 * device number >= 1.
 * @returns 0 on success, -10 if no matching device is found.
 */
int resolveUSBSoundDevice(const char* matchPortPath, int& alsaCard, std::string& ossDevice);

/**
 * Takes the path of a physical USB device and produces the HID device identifies.
 * This is tricky because the mapping depends on the USB enumeration process which
 * can change depending on conditions.
* 
 * @param matchPortPath The physical USB port path or null if any 
 * port is acceptable.
 * @param hidDevice Gets a string of the format "/dev/hidrawX" where X is and integer.
 * @returns 0 on success, -10 if no matching device is found.
 */
int resolveUSBHIDDevice(const char* matchPortPath, std::string& hidDevice);

/**
 * A utility function for getting the hex vendor ID from a human-readable
 * vendor name.
 * 
 * @returns 0 on success, -10 when not found.
 */
int resolveVendorName(const char* targetName, std::string& vendorId);

/**
 * Converts vendor/product ID pair into the corresponding full name pair.
 * 
 * @param vendorName Will be filled with a null-terminated name, as much as
 *   will fit.
 * @param productName Will be filled with a null-terminated name, as much as
 *   will fit.
 * @returns 0 if found, -1 if not found
 */
int getVendorAndProductName(const char* targetVendorId, const char* targetProductId, 
    std::string& vendorName, std::string& productName);

/**
 * Iterates across all USB devices and calls the callback for each one.
 * @param userData Will be passed back in the callback function.
 */
int visitUSBDevices(std::function<void(const char* vendorId, const char* productId, 
    const char* portPath, int usbBus, int usbDevice)> cb);

/**
 * Iterates across all USB devices and calls the callback for each one.
 */
int visitUSBDevices2(std::function<void(const char* vendorName, const char* productName, 
    const char* vendorId, const char* productId, const char* portPath,
    int usbBus, int usbDevice)> cb);
    
}
