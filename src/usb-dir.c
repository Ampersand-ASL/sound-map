/*
#	List of USB ID's
#
#	Maintained by Stephen J. Gowdy <linux.usb.ids@gmail.com>
#	If you have any new entries, please submit them via
#		http://www.linux-usb.org/usb-ids.html
#	or send entries as patches (diff -u old new) in the
#	body of your email (a bot will attempt to deal with it).
#	The latest version can be obtained from
#		http://www.linux-usb.org/usb.ids
#
# Version: 2025.12.13
# Date:    2025-12-13 20:34:01
#
*/
#include "usb-dir.h"

// IMPORTANT: This is a partial list extracted from here:
//
// http://www.linux-usb.org/usb.ids
//
// TODO: At some point we will create a script that can pull the entire list.

unsigned UsbDirLen = 30;

struct NameDirectoryEntry UsbDir[] = {
    { .level=0, .id="0d8c", .name="C-Media Electronics, Inc.", .childCount=25 },
        { .level=1, .id="0001", .name="Audio Device", .childCount=0 },
        { .level=1, .id="0002", .name="Composite Device", .childCount=0 },
        { .level=1, .id="0003", .name="Sound Device", .childCount=0 },
        { .level=1, .id="0004", .name="CM6631A Audio Processor", .childCount=0 },
        { .level=1, .id="0005", .name="Blue Snowball", .childCount=0 },
        { .level=1, .id="0006", .name="Storm HP-USB500 5.1 Headset", .childCount=0 },
        { .level=1, .id="000c", .name="Audio Adapter", .childCount=0 },
        { .level=1, .id="000d", .name="Composite Device", .childCount=0 },
        { .level=1, .id="000e", .name="Audio Adapter (Planet UP-100, Genius G-Talk)", .childCount=0 },
        { .level=1, .id="0012", .name="USB Audio Device", .childCount=0 },
        { .level=1, .id="0014", .name="Audio Adapter (Unitek Y-247A)", .childCount=0 },
        { .level=1, .id="001f", .name="CM108 Audio Controller", .childCount=0 },
        { .level=1, .id="0102", .name="CM106 Like Sound Device", .childCount=0 },
        { .level=1, .id="0103", .name="CM102-A+/102S+ Audio Controller", .childCount=0 },
        { .level=1, .id="0104", .name="CM103+ Audio Controller", .childCount=0 },
        { .level=1, .id="0105", .name="CM108 Audio Controller", .childCount=0 },
        { .level=1, .id="0107", .name="CM108 Audio Controller", .childCount=0 },
        { .level=1, .id="010f", .name="CM108 Audio Controller", .childCount=0 },
        { .level=1, .id="0115", .name="CM108 Audio Controller", .childCount=0 },
        { .level=1, .id="0139", .name="Multimedia Headset [Gigaware by Ignition L.P.]", .childCount=0 },
        { .level=1, .id="013c", .name="CM108 Audio Controller", .childCount=0 },
        { .level=1, .id="0201", .name="CM6501", .childCount=0 },
        { .level=1, .id="5000", .name="Mass Storage Controller", .childCount=0 },
        { .level=1, .id="5200", .name="Mass Storage Controller(0D8C,5200)", .childCount=0 },
        { .level=1, .id="b213", .name="USB Phone CM109 (aka CT2000,VPT1000)", .childCount=0 },
    { .level=0, .id="1d6b", .name="Linux Foundation", .childCount=3 },
        { .level=1, .id="0001", .name="1.1 root hub", .childCount=0 },
        { .level=1, .id="0002", .name="2.0 root hub", .childCount=0 },
        { .level=1, .id="0003", .name="2.0 root hub", .childCount=0 },
};
