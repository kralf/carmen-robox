#include <string.h>

#include <carmen/carmen.h>

#include "carmen_laser_device.h"
#include "carmen_sick.h"

//array of laser devices
int carmen_laser_devices_num;
carmen_laser_device_t* carmen_laser_devices[MAX_LASER_DEVICES];


//configurations
int carmen_laser_configurations_num;
carmen_laser_laser_config_t carmen_laser_configurations[MAX_LASER_CONFIGURATIONS];


carmen_laser_device_t* carmen_create_laser_instance(carmen_laser_laser_config_t* config, int laser_id, char* filename){
  carmen_laser_device_t* device=NULL;
  if (config->laser_type==SICK_LMS){
    device=carmen_create_sick_instance(config, laser_id);
  }
  if (device){
    device->config=*config;
    strcpy(device->device_name, filename);
  }
  return device;
}

int carmen_laser_register_devices(void){
  int c=0;
  c+=carmen_init_sick_configs();
  return c;
}
