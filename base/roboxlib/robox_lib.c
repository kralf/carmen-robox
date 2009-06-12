/***************************************************************************
 *   Copyright (C) 2009 by Ralf Kaestner                                   *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <carmen/carmen.h>
#include <carmen/drive_low_level.h>

#include "robox.h"

int robox_model;

double robox_control_freq;

char* robox_security_estop_dev;
char* robox_security_sstop_dev;
char* robox_security_watchdog_dev;
char* robox_security_flashlight_dev;
char* robox_power_engage_dev;
char* robox_power_battery_dev;
char* robox_sensors_check_dev;
char* robox_sensors_ok_dev;
char* robox_encoder_right_dev;
char* robox_encoder_left_dev;
char* robox_bumper_dev_dir;
char* robox_brake_disengage_dev;
char* robox_brake_disengaged_dev;
char* robox_motor_enable_dev;
char* robox_motor_right_dev;
char* robox_motor_left_dev;

robox_robot_t robox_robot;

int robox_read_parameters(int argc, char **argv) {
  int num_params;
  carmen_param_t robox_params[] = {
    {"robox", "control_freq", CARMEN_PARAM_DOUBLE, 
      &robox_control_freq, 0, NULL},

    {"robox", "security_estop_dev", CARMEN_PARAM_STRING, 
      &robox_security_estop_dev, 0, NULL},
    {"robox", "security_sstop_dev", CARMEN_PARAM_STRING, 
      &robox_security_sstop_dev, 0, NULL},
    {"robox", "security_watchdog_dev", CARMEN_PARAM_STRING, 
      &robox_security_watchdog_dev, 0, NULL},
    {"robox", "security_flashlight_dev", CARMEN_PARAM_STRING, 
      &robox_security_flashlight_dev, 0, NULL},
    {"robox", "power_engage_dev", CARMEN_PARAM_STRING, 
      &robox_power_engage_dev, 0, NULL},
    {"robox", "power_battery_dev", CARMEN_PARAM_STRING, 
      &robox_power_battery_dev, 0, NULL},
    {"robox", "sensors_check_dev", CARMEN_PARAM_STRING, 
      &robox_sensors_check_dev, 0, NULL},
    {"robox", "sensors_ok_dev", CARMEN_PARAM_STRING, 
      &robox_sensors_ok_dev, 0, NULL},
    {"robox", "encoder_right_dev", CARMEN_PARAM_STRING, 
      &robox_encoder_right_dev, 0, NULL},
    {"robox", "encoder_left_dev", CARMEN_PARAM_STRING, 
      &robox_encoder_left_dev, 0, NULL},
    {"robox", "bumper_dev_dir", CARMEN_PARAM_STRING, 
      &robox_bumper_dev_dir, 0, NULL},
    {"robox", "brake_disengage_dev", CARMEN_PARAM_STRING, 
      &robox_brake_disengage_dev, 0, NULL},
    {"robox", "brake_disengaged_dev", CARMEN_PARAM_STRING, 
      &robox_brake_disengaged_dev, 0, NULL},
    {"robox", "motor_enable_dev", CARMEN_PARAM_STRING, 
      &robox_motor_enable_dev, 0, NULL},
    {"robox", "motor_right_dev", CARMEN_PARAM_STRING, 
      &robox_motor_right_dev, 0, NULL},
    {"robox", "motor_left_dev", CARMEN_PARAM_STRING, 
      &robox_motor_left_dev, 0, NULL},
  };

  num_params = sizeof(robox_params)/sizeof(carmen_param_t);
  carmen_param_install_params(argc, argv, robox_params, num_params);

  return num_params;
}

int carmen_base_direct_sonar_on(void) {
  carmen_warn("%s not supported by robox.\n", __FUNCTION__);
  return 0;
}

int carmen_base_direct_sonar_off(void) {
  return 0;
}

int carmen_base_direct_reset(void) {
  return 0;
}

int carmen_base_direct_initialize_robot(char *model, char *dev __attribute__ 
  ((unused))) {
  char* argv[1] = {"robox"};

  if (robox_read_parameters(1, argv) > 0) {
    config_t config;
    config_init(&config);
    
    if (!carmen_strcasecmp(model, "robox"))
      config_set_int(&config, ROBOX_PARAMETER_MODEL, robox_model_robox);
    else if (!carmen_strcasecmp(model, "biba"))
      config_set_int(&config, ROBOX_PARAMETER_MODEL, robox_model_biba);
    else {
      carmen_warn("%s Unknown RoboX model %s:\nAccepted models are\n"
        "ROBOX and BIBA%s\n", carmen_red_code, model, carmen_normal_code);
      return -1;
    }

    config_set_string(&config, ROBOX_PARAMETER_SECURITY_ESTOP_DEV, 
      robox_security_estop_dev);
    config_set_string(&config, ROBOX_PARAMETER_SECURITY_SSTOP_DEV, 
      robox_security_sstop_dev);
    config_set_string(&config, ROBOX_PARAMETER_SECURITY_WATCHDOG_DEV, 
      robox_security_watchdog_dev);
    config_set_string(&config, ROBOX_PARAMETER_SECURITY_FLASHLIGHT_DEV, 
      robox_security_flashlight_dev);
    config_set_string(&config, ROBOX_PARAMETER_POWER_ENGAGE_DEV, 
      robox_power_engage_dev);
    config_set_string(&config, ROBOX_PARAMETER_POWER_BATTERY_DEV, 
      robox_power_battery_dev);
    config_set_string(&config, ROBOX_PARAMETER_SENSORS_CHECK_DEV, 
      robox_sensors_check_dev);
    config_set_string(&config, ROBOX_PARAMETER_SENSORS_OK_DEV, 
      robox_sensors_ok_dev);
    config_set_string(&config, ROBOX_PARAMETER_ENCODER_RIGHT_DEV, 
      robox_encoder_right_dev);
    config_set_string(&config, ROBOX_PARAMETER_ENCODER_LEFT_DEV, 
      robox_encoder_left_dev);
    config_set_string(&config, ROBOX_PARAMETER_BUMPER_DEV_DIR, 
      robox_bumper_dev_dir);
    config_set_string(&config, ROBOX_PARAMETER_BRAKE_DISENGAGE_DEV, 
      robox_brake_disengage_dev);
    config_set_string(&config, ROBOX_PARAMETER_BRAKE_DISENGAGED_DEV, 
      robox_brake_disengaged_dev);
    config_set_string(&config, ROBOX_PARAMETER_MOTOR_ENABLE_DEV, 
      robox_motor_enable_dev);
    config_set_string(&config, ROBOX_PARAMETER_MOTOR_RIGHT_DEV, 
      robox_motor_right_dev);
    config_set_string(&config, ROBOX_PARAMETER_MOTOR_LEFT_DEV, 
      robox_motor_left_dev);

    if (robox_init(&robox_robot, &config) ||
      robox_start(&robox_robot, robox_control_freq))
      return -1;
  }
  else
    return -1;

  return 0;
}

int carmen_base_direct_shutdown_robot(void) {
  if (!robox_stop(&robox_robot)) {
    robox_destroy(&robox_robot);
    return 0;
  }
  else
    return -1;
}

int carmen_base_direct_set_acceleration(double acceleration) {
  acceleration = 0.0;
  return 0;
}

int carmen_base_direct_set_deceleration(double deceleration) {
  deceleration = 0.0;
  return 0;
}

int carmen_base_direct_set_velocity(double tv, double rv) {
  tv = 0.0;
  rv = 0.0;

  return 0;
}

int carmen_base_direct_update_status(double* update_timestamp) {
  if (update_timestamp)
    *update_timestamp = 0.0;

  return 0;
}

int carmen_base_direct_get_state(double *displacement, double *rotation,
  double *tv, double *rv) {
  if (displacement)
    *displacement = 0.0;
  if (rotation)
    *rotation = 0.0;
  if (tv)
    *tv = 0.0;
  if (rv)
    *rv = 0.0;

  return 0;
}

int carmen_base_direct_get_integrated_state(double *x __attribute__ ((unused)), 
  double *y __attribute__ ((unused)), double *theta __attribute__ ((unused)), 
  double *tv __attribute__ ((unused)), double *rv __attribute__ ((unused))) {
  carmen_warn("%s not supported by robox.\n", __FUNCTION__);
  return 0;
}

int carmen_base_direct_send_binary_data(unsigned char *data __attribute__ 
  ((unused)), int size __attribute__ ((unused))) {
  carmen_warn("%s not supported by robox.\n", __FUNCTION__);
  return 0;
}

int carmen_base_direct_get_binary_data(unsigned char **data, int *size) {
  *data = 0;
  *size = 0;

  return 0;
}

int carmen_base_direct_get_bumpers(unsigned char *state, int num_bumpers) {
  if (!state || !num_bumpers)
    return 8;

  int i;
  if (num_bumpers > 8) 
    num_bumpers = 8;

  for (i = 0; i < num_bumpers; i++)
    state[i] = 0;

  return num_bumpers;
}

void carmen_base_direct_arm_get(double servos[] __attribute__ ((unused)), 
  int num_servos __attribute__ ((unused)), double 
  *currents __attribute__ ((unused)), int *gripper __attribute__ ((unused))) {
  carmen_warn("%s not supported by robox.\n", __FUNCTION__);
}

void carmen_base_direct_arm_set(double servos[] __attribute__ ((unused)), 
  int num_servos __attribute__ ((unused))) {
  carmen_warn("%s not supported by robox.\n", __FUNCTION__);
}

int carmen_base_direct_get_sonars(double *ranges __attribute__ ((unused)), 
  carmen_point_t *positions __attribute__ ((unused)), int num_sonars 
  __attribute__ ((unused))) {
  carmen_warn("%s not supported by robox.\n", __FUNCTION__);
  return 0;
}