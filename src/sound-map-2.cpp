#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <strings.h>

#include <cassert>
#include <functional>

#include "sound-map.h"
#include "usb-dir.h"

extern unsigned UsbDirLen;
extern struct NameDirectoryEntry UsbDir[];

/**
 * Visits all entries in the USB directory.
 * Data credit: http://www.linux-usb.org/usb.ids
 */
void visitVendorProductDirectory(
    std::function<bool(const char*, const char*, const char*, const char*)> cb) {
    char vendorId[5], productId[5], vendorName[49], productName[49];
    for (unsigned i = 0; i < UsbDirLen; i++) {
        if (UsbDir[i].level == 0) {
            strncpy(vendorId, UsbDir[i].id, 4);
            vendorId[4] = 0;
            strncpy(vendorName, UsbDir[i].name, 48);
            vendorName[48] = 0;
        } else {
            strncpy(productId, UsbDir[i].id, 4);
            productId[4] = 0;
            strncpy(productName, UsbDir[i].name, 48);
            productName[48] = 0;
            bool b = cb(vendorId, vendorName, productId, productName);
            if (!b)
                return;
        }
    }
}

extern "C" {

int resolveVendorName(const char* targetName, char* vId, unsigned vIdLen) {
    bool hit = false;
    visitVendorProductDirectory([&](
        const char* vId2, const char* vName, 
        const char* pId2, const char* pName) {
        if (strcasecmp(targetName, vName) == 0) {
            snprintf(vId, vIdLen, "%s", vId2);
            hit = true;
            return false;
        } else {
            return true;
        }
    });    
    if (hit)
        return 0;
    else 
        return -10;
}

void getVendorAndProductName(
    const char* targetVId, const char* targetPId, 
    char* vendorName, unsigned vendorNameLen, 
    char* productName, unsigned productNameLen) {

    visitVendorProductDirectory([&](
        const char* vId, const char* vName, 
        const char* pId, const char* pName) {
        if (strcasecmp(targetVId, vId) == 0 && strcasecmp(targetPId, pId) == 0) {
            snprintf(vendorName, vendorNameLen, "%s", vName);
            snprintf(productName, productNameLen, "%s", pName);
            return false;
        } else {
            return true;
        }
    });    
}

}
