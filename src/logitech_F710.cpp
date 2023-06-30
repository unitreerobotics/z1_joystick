#include "joystick/logitech_F710.h"

#ifdef COMPILE_WITH_SIMULATION

namespace unitree_joy {

JoyF710::JoyF710() : Joystick("F710")
{
  _sub = _nh.subscribe<sensor_msgs::Joy>("joy", 10, &JoyF710::joyCallBack, this);
  _updateThread = std::make_shared<UNITREE_ARM::LoopFunc>("readJoystick", 0.01, 
    [this](){
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
  /* buttons */
  joyData_.btn.back(joy->buttons[8]);
  joyData_.btn.start(joy->buttons[9]);
  joyData_.btn.LS(joy->buttons[10]);
  joyData_.btn.RS(joy->buttons[11]);
  joyData_.btn.LB(joy->buttons[4]);
  joyData_.btn.RB(joy->buttons[5]);
  joyData_.btn.A(joy->buttons[1]);
  joyData_.btn.B(joy->buttons[2]);
  joyData_.btn.X(joy->buttons[0]);
  joyData_.btn.Y(joy->buttons[3]);
  joyData_.btn.up(joy->axes[5] > 0.5 ? 1 : 0);
  joyData_.btn.down(joy->axes[5] <-0.5 ? 1 : 0);
  joyData_.btn.left(joy->axes[4] > 0.5 ? 1 : 0);
  joyData_.btn.right(joy->axes[4] <-0.5 ? 1 : 0);

  /* axes */
  joyData_.axes.lx(joy->axes[0]);
  joyData_.axes.ly(joy->axes[1]);
  joyData_.axes.rx(joy->axes[2]);
  joyData_.axes.ry(joy->axes[3]);
  joyData_.axes.LT(joy->buttons[6]);
  joyData_.axes.RT(joy->buttons[7]);
}

} // namespace unitree_joy

#endif // COMPILE_WITH_SIMULATION