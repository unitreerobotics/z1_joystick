
#ifndef _LOGITECH_F710_H
#define _LOGITECH_F710_H

/**
 * @file logitech_F710.h
 * Contains  logitech F710 joystick 
 */

#ifdef COMPILE_WITH_SIMULATION


#include "unitree_arm_sdk/control/unitreeArm.h"
#include "joystick/joystick.h"
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>

namespace unitree_joy {

/**
 * @brief Maintains a connection to the Logitech F710 Joystick, 
 * provides the current joystick data by rostopic.
 * 
 * @attention used only when simulation
 * terminal 1: roscore
 * terminal 2: rosrun joy joy_node
 */
class JoyF710 : public Joystick
{
public:
  JoyF710();
  ~JoyF710();

private:
  void joyCallBack(const sensor_msgs::Joy::ConstPtr& joy);

  ros::NodeHandle _nh;
  ros::Subscriber _sub;
  std::shared_ptr<UNITREE_ARM::LoopFunc> _updateThread;
};

} // namespace unitree_joy

#endif // COMPILE_WITH_SIMULATION

#endif // _LOGITECH_F710_H
