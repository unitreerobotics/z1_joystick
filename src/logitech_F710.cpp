#include "joystick/logitech_F710.h"

#ifdef COMPILE_WITH_SIMULATION

namespace unitree_joy {

JoyF710::JoyF710() : Joystick("F710")
{
  _sub = _nh.subscribe<sensor_msgs::Joy>("joy", 10, &JoyF710::joyCallBack, this);
  _updateThread = std::make_shared<UNITREE_ARM::LoopFunc>("readJoystick", 0.01, 
    [this](){
      _joyDataPre = _joyData;
      ros::spinOnce();
    });
  _updateThread->start();
}

JoyF710::~JoyF710()
{
  _updateThread->shutdown();
  ros::shutdown();
}

/**
 * @brief Changes the ros joy data to unitree_joy::JoystickDataStruct
 */
void JoyF710::joyCallBack(const sensor_msgs::Joy::ConstPtr& joy)
{
  //btn
  _joyData.btn.back  = joy->buttons[8];
  _joyData.btn.start = joy->buttons[9];
  _joyData.btn.LS    = joy->buttons[10];
  _joyData.btn.RS    = joy->buttons[11];
  _joyData.btn.LB    = joy->buttons[4];
  _joyData.btn.RB    = joy->buttons[5];
  _joyData.btn.A     = joy->buttons[1];
  _joyData.btn.B     = joy->buttons[2];
  _joyData.btn.X     = joy->buttons[0];
  _joyData.btn.Y     = joy->buttons[3];
  _joyData.btn.up    = joy->axes[5] > 0.5 ? 1 : 0;
  _joyData.btn.down  = joy->axes[5] <-0.5 ? 1 : 0;
  _joyData.btn.left  = joy->axes[4] > 0.5 ? 1 : 0;
  _joyData.btn.right = joy->axes[4] <-0.5 ? 1 : 0;

  //axes
  _joyData.axes.lx = joy->axes[0];
  _joyData.axes.ly = joy->axes[1];
  _joyData.axes.rx = joy->axes[2];
  _joyData.axes.ry = joy->axes[3];
  _joyData.axes.LT = joy->buttons[6];
  _joyData.axes.RT = joy->buttons[7];
}

} // namespace unitree_joy

#endif // COMPILE_WITH_SIMULATION