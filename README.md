usb.cpp uses sd-device library to get information about connected usb-storages from linux-based system. Then opens system file /proc/mounts to find mounted path to the contents of a usb drive. And outputs the path to the standard output.