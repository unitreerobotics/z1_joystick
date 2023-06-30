#include "joystick/joy_controller.h"

namespace unitree_joy {

JoyController::JoyController(std::string joyName)
{
  if(joyName.compare("Aliengo") == 0){
    _joy = std::make_shared<UnitreeJoystick>(DogType::Aliengo);
  }else if(joyName.compare("B1") == 0){
    _joy = std::make_shared<UnitreeJoystick>(DogType::B1);
  }else if(joyName.compare("F710") == 0){
#ifdef COMPILE_WITH_SIMULATION
    _joy = std::make_shared<JoyF710>();
#else
    std::cout << "[ERROR] Please enable ros to use F710."<<std::endl;
    exit(0);
#endif
  }
}

UNITREE_ARM::ArmFSMState JoyController::getSwitchState()
{
  // If LT and RT are pressed at the same time, 
  // the joystick data will be no longer responded until RB or RT is pressed again later.
  if(_joy->axes().RT.isPressed()) {
    if(_joy->axes().LT.isPressed()) {
      _lock = true;
    }
    if(_joy->btn().RB.isPressed()) {
      return UNITREE_ARM::ArmFSMState::PASSIVE;
    }  
  }
  if(_joy->axes().RT.isJustPressed()) 
  {
    _lock = false;
    return UNITREE_ARM::ArmFSMState::JOINTCTRL;
  }
  if(_joy->btn().RB.isJustPressed() && (!_lock)) 
  {
    return UNITREE_ARM::ArmFSMState::CARTESIAN;
  }
  if(_joy->btn().back.isJustPressed() && (!_lock)) 
  {
    return UNITREE_ARM::ArmFSMState::BACKTOSTART;
  }


  if(_joy->axes().RT.isPressed()) {
    /* set trejectory number, RT+button */
    if(_joy->btn().up.isPressed()) {
      _trajNum = 1;
    } else if(_joy->btn().down.isPressed()) {
      _trajNum = 2;
    } else if(_joy->btn().left.isPressed()) {
      _trajNum = 3;
    } else if(_joy->btn().right.isPressed()) {
      _trajNum = 4;
    } else if(_joy->btn().Y.isPressed()) {
      _trajNum = 5;
    } else if(_joy->btn().A.isPressed()) {
      _trajNum = 6;
    } else if(_joy->btn().X.isPressed()) {
      _trajNum = 7;
    } else if(_joy->btn().B.isPressed()) {
      _trajNum = 8;
    }
    if(_trajNum != 0){
      return UNITREE_ARM::ArmFSMState::TRAJECTORY;
    }
  }

  return UNITREE_ARM::ArmFSMState::INVALID;
}

double JoyController::getSpeedAdd()
{
  return _joy->btn().Y.isJustPressed() ? 1.0 : (_joy->btn().A.isJustPressed() ? -1.0 : 0.0);
}

Vec7 JoyController::getDirections(UNITREE_ARM::ArmFSMState state)
{
  // Combines two axes to a direction
  auto axesToDir = [](double axis_data, double threhold) -> double 
  {
    if(fabs(axis_data)>threhold){
      return axis_data > 0 ? 1.0 : -1.0;
    }
    return 0.0;
  };

  // Combines two buttons to a direction
  // The first one represents the positive direction
  auto btnToDir = [](uint8_t btn1, uint8_t btn2) -> double {
    return btn1 == 1 ? 1.0 : ( btn2 == 1 ? -1.0 : 0.0 );
  };

  Vec7 directions = Vec7::Zero();
  if(!_lock)
  {
    switch (state)
    {
    case UNITREE_ARM::ArmFSMState::JOINTCTRL:
      directions << // J1, J2, J3, J4, J5, J6, gripper
       -axesToDir(_joy->axes().lx(), joyThre),
        axesToDir(_joy->axes().ly(), joyThre),
       -btnToDir(_joy->btn().up(), _joy->btn().down()),
       -axesToDir(_joy->axes().ry(), joyThre),
       -axesToDir(_joy->axes().rx(), joyThre),
       -btnToDir(_joy->btn().X(), _joy->btn().B()),
       -btnToDir(_joy->btn().left(), _joy->btn().right()) * 2 // move gripper faster
        ;
      break;
    case UNITREE_ARM::ArmFSMState::CARTESIAN:
      directions << // rx, ry, rz, x, y, z, gripper
        axesToDir(_joy->axes().rx(), joyThre),
        axesToDir(_joy->axes().ry(), joyThre),
        btnToDir(_joy->btn().X(), _joy->btn().B()),
        axesToDir(_joy->axes().ly(), joyThre),
       -axesToDir(_joy->axes().lx(), joyThre),
        btnToDir(_joy->btn().up(), _joy->btn().down()),
       -btnToDir(_joy->btn().left(), _joy->btn().right()) * 2 // move gripper faster
        ;
      break;
    }
  }
  return directions;
}

// Stores the trajectory number until invoking the function to read, and then sets to zero.
int JoyController::getTrajNum()
{
  int trajNum = _trajNum;
  _trajNum = 0;
  return trajNum;
}

}