/* List mounted USB storage using sd-device
* systemd library must be installed: sudo apt install libsystemd-dev 
*  g++ usb.cpp -o out `pkg-config --cflags --libs libsystemd`
* ./out
*/


#include <iostream>
#include <systemd/sd-device.h>
#include <fstream>
#include <string>
#include <vector>

void get_usb_storage_devname_list(std::vector<std::string>& device_devname_list)
{
  sd_device* device;
  sd_device_enumerator* e;
  sd_device_enumerator_new(&e);
  const char *dev_name, *dev_type, *storage_type;

  sd_device_enumerator_add_match_subsystem(e, "block", int(1));
  for(device = sd_device_enumerator_get_device_first(e);
      device != NULL; device = sd_device_enumerator_get_device_next(e))
  {
    sd_device_get_property_value(device, "ID_BUS", &storage_type);
    sd_device_get_devtype(device, &dev_type);

    if(dev_type == std::string("partition") &&
         storage_type == std::string("usb"))
    {
     sd_device_get_devname(device, &dev_name);
     device_devname_list.push_back(dev_name);
    }
  }

}

void find_mounted_devices(const std::vector<std::string>& usb_device_devname_list, 
	std::vector<std::string>& usb_device_mounted_path_list)
{
  std::ifstream mount_list_handler;
  std::string mount_device;

  mount_list_handler.open("/proc/mounts");
  if(mount_list_handler.is_open())
  {
   while(getline(mount_list_handler, mount_device))
   {
    for(auto name : usb_device_devname_list)
    {
     if(name == mount_device.substr(int(0), mount_device.find_first_of(" ")))
     {
      auto path = mount_device.substr(mount_device.find_first_of(" ") + int(1));
      usb_device_mounted_path_list.push_back(path.substr(int(0), path.find_first_of(" ")));
     }
    }
   }
   mount_list_handler.close();
  }
  else
  {
    std::cerr << "Unable to open the mount list" << std::endl;
  }

//check for unmounted devices
 while(usb_device_mounted_path_list.size() < usb_device_devname_list.size())
 {
  usb_device_mounted_path_list.push_back("Unmounted");
 }
}

int main()
{
 std::vector<std::string> usb_devname_list;
 std::vector<std::string> usb_mount_list;

 get_usb_storage_devname_list(usb_devname_list);
 find_mounted_devices(usb_devname_list, usb_mount_list);

 if(!usb_devname_list.empty())
 {
  for(auto& path : usb_mount_list)
  {
   std::cout << path << std::endl;
  }
 }
 else
 {
  std::cout << "No usb storage is found" << std::endl;
 }

  return 0;
}
