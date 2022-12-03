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


int main()
{
  sd_device* device;
  sd_device_enumerator* e;
  sd_device_enumerator_new(&e);
  const char *dev_name, *dev_type, *storage_type;
  std::vector<std::string> device_devname_list;
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
  std::ifstream mount_list_handler;
  std::string mount_device;
  bool device_is_found = false;

  mount_list_handler.open("/proc/mounts");
  if(mount_list_handler.is_open())
  {
   while(getline(mount_list_handler, mount_device))
   {
    for(auto name : device_devname_list)
    {
     if(name ==
	mount_device.substr(int(0), mount_device.find_first_of(" ")))
     {
      device_is_found = true;
      auto path = mount_device.substr(mount_device.find_first_of(" ") + int(1));
      std::cout << path.substr(int(0), path.find_first_of(" "))  << std::endl;
     }
    }
   }
   mount_list_handler.close();
  }
  else
  {
    std::cerr << "Unable to open the mount list" << std::endl;
  }
 if(!device_is_found) std::cout << "No usb storage is found" << std::endl;

  return 0;
}
