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
  if(_joy->getdata().axes.LT > joyThre && _joy->getdata().axes.RT > joyThre){
      if(!_lock){
        // printf("lock\n");
        _lock = true;
      }
  }
  if(_joy->getdata().axes.LT > joyThre){
    if(_joy->getdata().btn.LB == 1) {
      return UNITREE_ARM::ArmFSMState::PASSIVE;
    }
  }

  if(_joy->getdata().axes.RT > joyThre){
    // Press RT for the first time
    if(_joy->getdataPre().axes.RT < joyThre){
      if(_lock){
        // printf("unlock\n");
        _lock = false;//unlock joystick
      }
      return UNITREE_ARM::ArmFSMState::JOINTCTRL;
    }
    //RT+RB
    if(_joy->getdata().btn.RB == 1){
      return UNITREE_ARM::ArmFSMState::PASSIVE;
    }

    //set trejectory number, RT+button
    if(_joy->getdata().btn.up == 1){
      _trajNum = 1;
    }else if(_joy->getdata().btn.down == 1){
      _trajNum = 2;
    }else if(_joy->getdata().btn.left == 1){
      _trajNum = 3;
    }else if(_joy->getdata().btn.right == 1){
      _trajNum = 4;
    }else if(_joy->getdata().btn.Y == 1){
      _trajNum = 5;
    }else if(_joy->getdata().btn.A == 1){
      _trajNum = 6;
    }else if(_joy->getdata().btn.X == 1){
      _trajNum = 7;
    }else if(_joy->getdata().btn.B == 1){
      _trajNum = 8;
    }
    
    if(_trajNum != 0){
      return UNITREE_ARM::ArmFSMState::TRAJECTORY;
    }
  }else if(_joy->getdata().btn.RB == 1){
    // Press RB for the first time
    if((_joy->getdataPre().btn.RB == 0)){
      if(_lock){
        // printf("unlock\n");
        _lock = false;//unlock joystick
      }
      return UNITREE_ARM::ArmFSMState::CARTESIAN;
    }
  }else if(_joy->getdata().btn.back == 1){
    // Press back for the first time
    if((_joy->getdataPre().btn.back == 0)){
      if(!_lock) return UNITREE_ARM::ArmFSMState::BACKTOSTART;
    }
  }

  return UNITREE_ARM::ArmFSMState::INVALID;
}

double JoyController::getSpeedAdd()
{
  if(_joy->getdata().btn.Y == 1){
    // Press Y for the first time
    if((_joy->getdataPre().btn.Y == 0)){
      return 1.0;
    }
  }else if(_joy->getdata().btn.A == 1){
    // Press A for the first time
    if((_joy->getdataPre().btn.A == 0)){
      return -1.0;
    }
  }
  return 0.0;
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
        axesToDir(_joy->getdata().axes.lx, joyThre),
        axesToDir(_joy->getdata().axes.ly, joyThre),
       -btnToDir(_joy->getdata().btn.up, _joy->getdata().btn.down),
       -axesToDir(_joy->getdata().axes.ry, joyThre),
        axesToDir(_joy->getdata().axes.rx, joyThre),
       -btnToDir(_joy->getdata().btn.X, _joy->getdata().btn.B),
       -btnToDir(_joy->getdata().btn.left, _joy->getdata().btn.right)*2 // move gripper faster
        ;
      break;
    case UNITREE_ARM::ArmFSMState::CARTESIAN:
      directions << // rx, ry, rz, x, y, z, gripper
        axesToDir(_joy->getdata().axes.rx, joyThre),
        axesToDir(_joy->getdata().axes.ry, joyThre),
        btnToDir(_joy->getdata().btn.X, _joy->getdata().btn.B),
        axesToDir(_joy->getdata().axes.ly, joyThre),
        axesToDir(_joy->getdata().axes.lx, joyThre),
        btnToDir(_joy->getdata().btn.up, _joy->getdata().btn.down),
       -btnToDir(_joy->getdata().btn.left, _joy->getdata().btn.right)*2 // move gripper faster
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