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

#ifdef WITH_ERA
#include <carmen/era_interface.h>
#endif

char* joystick_dev;
int joystick_axis_long;
int joystick_axis_lat;
int joystick_btn_deadman;
int joystick_btn_activate;
int joystick_btn_arm_stop;
int joystick_btn_arm_close;
int joystick_btn_arm_brace;

double robot_max_tv;
double robot_max_rv;

carmen_joystick_type joystick;
int joystick_activated = 0;

int joystick_verbose = 0;

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

  if (joystick_verbose)
    fprintf(stdout, "tv: %5.2f m/s  rv: %5.2f rad/s\n", v.tv, v.rv);
  
  err = IPC_publishData(CARMEN_BASE_VELOCITY_NAME, &v);
  carmen_test_ipc(err, "Could not publish", CARMEN_BASE_VELOCITY_NAME);
}

void sig_handler(int x) {
  if (x == SIGINT) {
    send_base_velocity_command(0.0, 0.0);
#ifdef WITH_ERA
    carmen_era_publish_stop_message(carmen_get_time());
#endif
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
    {"joystick", "button_arm_stop", CARMEN_PARAM_INT, &joystick_btn_arm_stop,
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
  char* joystick_arg_dev = 0;
  int err;
  double cmd_tv = 0, cmd_rv = 0;
  double timestamp;

  if ((argc > 1) && (!strcmp(argv[1], "-v")))
    joystick_verbose = 1;
  if ((argc == 2) || ((argc == 3) && (joystick_verbose)))
    joystick_arg_dev = argv[1+joystick_verbose];
  else {
    fprintf(stderr, "usage: %s [-v] [DEV]\n", argv[0]);
    return -1;
  }

  carmen_ipc_initialize(argc, argv);
  carmen_param_check_version(argv[0]);
  read_parameters(argc, argv);

  if (joystick_arg_dev)
    joystick_dev = joystick_arg_dev;

  err = IPC_defineMsg(CARMEN_BASE_VELOCITY_NAME, IPC_VARIABLE_LENGTH,
                      CARMEN_BASE_VELOCITY_FMT);
  carmen_test_ipc_exit(err, "Could not define", CARMEN_BASE_VELOCITY_NAME);
  
  signal(SIGINT, sig_handler);

  fprintf(stderr,"Looking for joystick at device: %s\n", joystick_dev);

  if (carmen_initialize_joystick_device(&joystick, joystick_dev) < 0)
    carmen_die("Error: could not find joystick at device: %s\n", joystick_dev);
  else
    fprintf(stderr,"Joystick has %d axes and %d buttons\n\n",
    joystick.nb_axes, joystick.nb_buttons);

  fprintf(stderr,"1. Center the joystick.\n");
  if (joystick_btn_activate <= 0) {
    fprintf(stderr,"2. The joystick is activated.\n");
    joystick_activated = 1;
  }
  else {
    fprintf(stderr,"2. Press button \"%d\" to activate/deactivate the "
      "joystick.\n", joystick_btn_activate);
  }
#ifdef WITH_ERA
  fprintf(stderr,"3. Press button \"%d\" to close in the arm,\n"
                 "   button \"%d\" to brace the arm.\n"
                 "   button \"%d\" to stop the arm.\n",
    joystick_btn_arm_close, joystick_btn_arm_brace, joystick_btn_arm_stop);
#endif
  if (joystick_btn_deadman > 0)
    fprintf(stderr,"4. Hold button \"%d\" to keep the robot moving.\n\n",
      joystick_btn_deadman);

  timestamp = carmen_get_time();
  while (1) {
    carmen_ipc_sleep(0.01);

    if (carmen_get_joystick_state(&joystick) >= 0) {
      if ((joystick_btn_activate > 0) &&
        joystick.buttons[joystick_btn_activate-1]) {
        joystick_activated = !joystick_activated;

        if (!joystick_activated) {
          send_base_velocity_command(0.0, 0.0);
#ifdef WITH_ERA
          carmen_era_publish_stop_message(carmen_get_time());
#endif
          fprintf(stderr,"Joystick deactivated.\n");
        }
        else
          fprintf(stderr,"Joystick activated.\n");
      }

      if (joystick_activated) {
        if ((joystick_btn_deadman <= 0) ||
          joystick.buttons[joystick_btn_deadman-1]) {
          cmd_tv = (joystick.axes[joystick_axis_long]) ?
            -joystick.axes[joystick_axis_long]/32767.0*robot_max_tv : 0.0;
          cmd_rv = (joystick.axes[joystick_axis_lat]) ?
            -joystick.axes[joystick_axis_lat]/32767.0*robot_max_rv : 0.0;
        }
        else {
          cmd_tv = 0.0;
          cmd_rv = 0.0;
        }

        send_base_velocity_command(cmd_tv, cmd_rv);

#ifdef WITH_ERA
        if (joystick.buttons[joystick_btn_arm_stop-1]) {
          carmen_era_publish_stop_message(
            carmen_get_time());
        }
        if (joystick.buttons[joystick_btn_arm_close-1]) {
          carmen_era_publish_joint_cmd_message(
            carmen_degrees_to_radians(0.0),
            carmen_degrees_to_radians(1.0),
            carmen_degrees_to_radians(0.0),
            carmen_degrees_to_radians(2.5),
            carmen_degrees_to_radians(-1.0),
            carmen_degrees_to_radians(0.0),
            0.5, carmen_get_time());
        }
        else if (joystick.buttons[joystick_btn_arm_brace-1]) {
          carmen_era_publish_joint_cmd_message(
            carmen_degrees_to_radians(0.0),
            carmen_degrees_to_radians(10.0),
            carmen_degrees_to_radians(0.0),
            carmen_degrees_to_radians(25.0),
            carmen_degrees_to_radians(-10.0),
            carmen_degrees_to_radians(0.0),
            0.5, carmen_get_time());
        }
#endif
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
