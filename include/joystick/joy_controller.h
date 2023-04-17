#ifndef _JOY_CONTROLLER_H
#define _JOY_CONTROLLER_H

#include "joystick/joystick.h"
#include "unitree_arm_sdk/control/unitreeArm.h"
#include "joystick/unitree_joystick.h"
#include "joystick/logitech_F710.h"

/**
 * @file joy_controller.h
 * Contains helper functions for getting control command for joystick data.
 */

namespace unitree_joy {

/**
 * @brief Maintains a connection to the joystick, 
 * gives access to extract z1 control commands from joystick data.
 */
class JoyController
{
public:
  /**
   * @brief Establishes a connection with the joystick.
   * 
   * @param joyName joystick name
   * 
   * "Aliengo" , "B1" or "F710"
   */
  JoyController(std::string joyName);
  ~JoyController(){};

  /**
   * @brief Get the Switch State object
   * 
   * @attention This function should be invoked in control loop every time
   * 
   * @return UNITREE_ARM::ArmFSMState 
   */
  UNITREE_ARM::ArmFSMState getSwitchState();

  /**
   * @brief Wether running speed needs to be changed.
   * 
   * @retval -1 decrease speed
   * @retval 0 no change
   * @retval 1 increase speed
   * 
   * @code {.cpp}
   * const double speed_increase = 0.1; // different for joint space or cartesian space
   * current_speed += speed_increase * getSpeedAdd();
   * current_speed = saturation(current_speed, LOWER_LIMIT, UPPER_LIMIT);
   * @endcode
   */
  double getSpeedAdd();

  /**
   * @brief Get the current moving directions 
   * 
   * @param state joint space or cartesian space
   * 
   * @retval Vec7_joint six joints and gripper
   * @retval Vec7_cartesian six dof and gripper
   */
  Vec7 getDirections(UNITREE_ARM::ArmFSMState state);

  /**
   * @brief Get the trajectory number
   * 
   * Use RT+button to trigger certain trajectory
   * 
   * @retval 1 RT+up
   * @retval 2 RT+down
   * @retval 3 RT+left
   * @retval 4 RT+right
   * @retval 5 RT+Y
   * @retval 6 RT+A
   * @retval 7 RT+X
   * @retval 8 RT+B
   */
  int getTrajNum();

private:
  std::shared_ptr<Joystick> _joy;
  UNITREE_ARM::ArmFSMState _fsmState;

  /**
   * @brief Whether to allow joystick control
   * if lock is true, the joystick no longer contols the movement of z1
   * 
   * true: when press LT+RT
   * false: when press RB or RT
   */
  bool _lock = false;
  const float joyThre = 0.5; // threhold to think axes as button
  int _trajNum = 0;
};

}

#endif // _JOY_CONTROLLER_H
