One of the problems that shows up sometimes with USB sound cards
on Linux is the dynamic way in which card/device numbers are assigned
by the various Linux device drivers. This applies to ALSA cards, OSS devices, 
and HID 
devices. Sometimes a system reboot, the addition or a new device, or just random 
chance can cause the devices to be re-numbered. This can cause an application
to malfunction if it is configured to use a sound device that suddenly 
changes its name.

This library provides a way to increase reliability by allowing a user
to specify a USB sound device using an physical USB bus number, a physical
USB port number, a USB vendor/product code or any combination of these.
The advantage of physical harware designations and vendor/product codes
is that they are immutable.

The querySoundMap() function takes a query string that look like this:

    vendor:0d8c
    vendor:0d8c,product:0001
    bus:3,vendor:0d8c
    bus:3,port:2

The first two cases look for devices identified by vendor codes. If only one device
with that code is attached to the system then everything is fine. If more than one
is connected (for example, two USB sound interfaces with the same CM108 chip inside)
then you will need to use a more specific query that identifies the physical port.




