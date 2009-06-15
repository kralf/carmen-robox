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
#include <carmen/joyctrl.h>
#include <carmen/era_interface.h>

char* joystick_dev;
int joystick_axis_long;
int joystick_axis_lat;
int joystick_btn_deadman;
int joystick_btn_activate;
int joystick_btn_arm_close;
int joystick_btn_arm_brace;

double robot_max_tv;
double robot_max_rv;

carmen_joystick_type joystick;
int joystick_activated = 0;

void send_base_velocity_command(double tv, double rv) {
  IPC_RETURN_TYPE err;
  static carmen_base_velocity_message v;

  v.tv = tv;
  v.rv = rv;
  v.timestamp = carmen_get_time();
  v.host = carmen_get_host();

  if (v.tv > robot_max_tv)
    v.tv = robot_max_tv;
  else if (v.tv < -robot_max_tv)
    v.tv = -robot_max_tv;

  if (v.rv > robot_max_rv)
    v.rv = robot_max_rv;
  else if (v.rv < -robot_max_rv)
    v.rv = -robot_max_rv;

  err = IPC_publishData(CARMEN_BASE_VELOCITY_NAME, &v);
  carmen_test_ipc(err, "Could not publish", CARMEN_BASE_VELOCITY_NAME);  
}

void sig_handler(int x) {
  if (x == SIGINT) {
    send_base_velocity_command(0.0, 0.0);

    carmen_close_joystick(&joystick);
    carmen_ipc_disconnect();

    printf("Disconnected from robot.\n");
    exit(0);
  }
}

void read_parameters(int argc, char **argv) {
  int num_params;
  
  carmen_param_t param_list[] = {
    {"joystick", "dev", CARMEN_PARAM_STRING, &joystick_dev, 0, NULL},
    {"joystick", "axis_longitudinal", CARMEN_PARAM_INT, &joystick_axis_long, 
      0, NULL},
    {"joystick", "axis_lateral", CARMEN_PARAM_INT, &joystick_axis_lat, 
      0, NULL},
    {"joystick", "button_deadman", CARMEN_PARAM_INT, &joystick_btn_deadman, 
      0, NULL},
    {"joystick", "button_activate", CARMEN_PARAM_INT, &joystick_btn_activate, 
      0, NULL},
    {"joystick", "button_arm_close", CARMEN_PARAM_INT, &joystick_btn_arm_close, 
      0, NULL},
    {"joystick", "button_arm_brace", CARMEN_PARAM_INT, 
      &joystick_btn_arm_brace, 0, NULL},

    {"robot", "max_t_vel", CARMEN_PARAM_DOUBLE, &robot_max_tv, 0, NULL},
    {"robot", "max_r_vel", CARMEN_PARAM_DOUBLE, &robot_max_rv, 0, NULL}
  };
  
  num_params = sizeof(param_list)/sizeof(param_list[0]);
  carmen_param_install_params(argc, argv, param_list, num_params);
}

int main(int argc, char **argv) {
  double cmd_tv = 0, cmd_rv = 0;
  double timestamp;

  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);
  read_parameters(argc, argv);
  signal(SIGINT, sig_handler);

  fprintf(stderr,"Looking for joystick at device: %s\n", joystick_dev);

  if (carmen_initialize_joystick(&joystick, joystick_dev) < 0)
    carmen_die("Error: could not find joystick at device: %s\n", joystick_dev);
  else
    fprintf(stderr,"Joystick has %d axes and %d buttons\n\n", 
    joystick.nb_axes, joystick.nb_buttons);
 
  fprintf(stderr,"1. Center the joystick.\n");
  fprintf(stderr,"2. Press button \"%d\" to activate/deactivate the "
    "joystick.\n", joystick_btn_activate);
  fprintf(stderr,"3. Press button \"%d\" to close in the arm,\n"
                 "   button \"%d\" to brace the arm." , 
    joystick_btn_arm_close, joystick_btn_arm_brace);
  if (joystick_btn_deadman > 0)
    fprintf(stderr,"4. Hold button \"%d\" to keep the robot moving.\n\n", 
      joystick_btn_deadman);

  timestamp = carmen_get_time();
  while (1) {
    carmen_ipc_sleep(0.1);

    if (carmen_get_joystick_state(&joystick) >= 0) {
      if (joystick.buttons[joystick_btn_activate-1]) {
        joystick_activated = !joystick_activated;

        if (!joystick_activated) {
          send_base_velocity_command(0.0, 0.0);
          fprintf(stderr,"Joystick deactivated.\n");
        }
        else
          fprintf(stderr,"Joystick activated.\n");
      }
      else if (joystick.buttons[joystick_btn_arm_close-1]) {
        carmen_era_publish_joint_cmd(0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.5,
          carmen_get_time());
      }
      else if (joystick.buttons[joystick_btn_arm_brace-1]) {
        carmen_era_publish_joint_cmd(0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.5,
          carmen_get_time());
      }

      if (joystick_activated) {
        if ((joystick_btn_deadman <= 0) || 
          joystick.buttons[joystick_btn_deadman-1]) {
          cmd_tv = (joystick.axes[joystick_axis_long]) ?
            joystick.axes[joystick_axis_long]/32767.0*robot_max_tv : 0.0;
          cmd_rv = (joystick.axes[joystick_axis_lat]) ?
            joystick.axes[joystick_axis_lat]/32767.0*robot_max_rv : 0.0;
        }
        else {
          cmd_tv = 0.0;
          cmd_rv = 0.0;
        }

        send_base_velocity_command(cmd_tv, cmd_rv);
      }
    }
    else if (joystick_activated && carmen_get_time()-timestamp > 0.5) {
      send_base_velocity_command(cmd_tv, cmd_rv);
      timestamp = carmen_get_time();
    }
  }
    
  sig_handler(SIGINT);
  return 0;
}
