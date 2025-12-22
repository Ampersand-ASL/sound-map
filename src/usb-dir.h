#ifndef _usb_dir_h
#define _usb_dir_h

struct NameDirectoryEntry {
    // 0 is the vendor level, 1 is the product level
    unsigned level;
    char id[4];
    char name[48];
    unsigned childCount;
};

#endif
