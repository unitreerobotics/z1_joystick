#include "control/armCtrl.h"

ArmCtrl::ArmCtrl(std::string joyName)
{
  _joy = std::make_shared<JoyController>(joyName);
  z1 = std::make_shared<unitreeArm>(true);
  _trajManager = std::make_shared<TrajectoryManager>(z1);


  // press RT+X to show default trajectory
  Vec6 show_left, show_mid, show_right;//copy from saveArmStates.csv
  show_mid << 0.0, 0.9, -1.2, 0.3, 0.0, 0.0;
  show_right << -1.0, 0.9, -1., -0.3, 0.0, 0.0;
  show_left << 1.0, 0.9, -1., -0.3, 0.0, 0.0;
  _trajManager->addMoveJ(jointQ2posture(show_mid), 0);
  _trajManager->addMoveJ(jointQ2posture(show_right), -1);
  _trajManager->addMoveJ(jointQ2posture(show_mid), 0);
  _trajManager->addMoveJ(jointQ2posture(show_left), -1);


  z1->sendRecvThread->start();
  _runThread = std::make_shared<LoopFunc>("arm_run", 0.004, boost::bind(&ArmCtrl::_run, this));
  _runThread->start();
}

void ArmCtrl::_run()
{
  // check real state
  switch (_joy->getSwitchState())
  {
  case ArmFSMState::BACKTOSTART:
    _currentState = ArmFSMState::BACKTOSTART;
    z1->setWait(true);
    z1->backToStart();
    break;
  case ArmFSMState::PASSIVE:
    z1->setFsm(ArmFSMState::PASSIVE);
    _currentState = _joy->getSwitchState();
    break;
  case ArmFSMState::JOINTCTRL:
  case ArmFSMState::CARTESIAN:
    _currentState = _joy->getSwitchState();
    z1->startTrack(_joy->getSwitchState());
    z1->directions.setZero();
    break;
  case ArmFSMState::TRAJECTORY:
    _currentState = _joy->getSwitchState();
    _trajManager->restart();
    break;
  default:
    // Most of time it is ArmFSMState::Invalid, 
    // until the joystick button has been pressed.
    break;
  }

  switch (_currentState)
  {
  case ArmFSMState::JOINTCTRL:
      _jointCtrl();
      break;
  case ArmFSMState::CARTESIAN:
      _cartesianCtrl();
      break;
  case ArmFSMState::TRAJECTORY:
      _trajectory(_joy->getTrajNum());
      break;
  case ArmFSMState::PASSIVE:
      z1->setFsm(_currentState);
      _currentState = ArmFSMState::INVALID;
    break;
  default:
      _checkForJointCtrl();
    break;
  }
}

void ArmCtrl::_jointCtrl()
{
  _jointSpeed += 0.1 * _joy->getSpeedAdd();
  _jointSpeed = saturation(_jointSpeed, 0.2, 1.0);

  // smooth the command
  const double filter_coe = 0.02;
  z1->directions = filter_coe* _joy->getDirections(_currentState)
     + (1-filter_coe)*z1->directions;
  z1->jointCtrlCmd(z1->directions, _jointSpeed);
}

void ArmCtrl::_cartesianCtrl()
{
  _cartesianSpeed += 0.05 * _joy->getSpeedAdd();
  _cartesianSpeed = saturation(_cartesianSpeed, 0.05, 0.2);

  auto new_directions = _joy->getDirections(_currentState);
  // Decrease the y-axis speed in singularity area
  if(z1->_ctrlComp->armModel->checkInSingularity(z1->lowstate->getQ())){
    new_directions(4) = new_directions(4) * 0.25;
  }

  // smooth the command
  const double filter_coe = 0.02;
  z1->directions = filter_coe* new_directions + (1-filter_coe)*z1->directions;
  z1->cartesianCtrlCmd(z1->directions, _cartesianSpeed*2, _cartesianSpeed);
}

void ArmCtrl::_trajectory(int trajNum)
{
  if(_trajManager->running)
  {
    _trajManager->run();
    return; // This trajectory has high priority.
  }

  z1->setWait(true);//block, joystick will be responded until the trajectory complete.
  Vec6 targetPos, targetQ;
  switch (trajNum)
  {
  case 1://up
    targetQ << 0, 1.5, -1, -0.54, 0, 0;
    z1->MoveJ(jointQ2posture(targetQ), 0., 1.5);
    break;
  case 2://down
    break;
  case 3://left
    break;
  case 4://right
    break;
  case 5://Y
    break;
  case 6://A
    break;
  case 7://X
    _trajManager->running = true;
    break;
  case 8://B
    break;
  default:
    break;
  }
}

void ArmCtrl::_checkForJointCtrl()
{
  if(z1->_ctrlComp->recvState.state == ArmFSMState::JOINTCTRL)
  {
    if(++cntJointState > 15)
    {
      _currentState = ArmFSMState::JOINTCTRL;
      z1->directions.setZero();
      cntJointState = 0;
    }
  }else{
    cntJointState = 0;
  }
}

/**         TrajectoryManager          */

void TrajectoryManager::addMoveJ(Vec6 endPosture, double gripper_angle)
{
  Vec6 qTemp;
  // Whether endPosture is in C-space
  if(_z1->_ctrlComp->armModel->inverseKinematics(
    postureToHomo(endPosture), Vec6::Zero(), qTemp, true)) {
    _trajectories.push_back(std::make_pair(endPosture, gripper_angle));
  }
}

void TrajectoryManager::run()
{
  if(_trajectories.empty()) 
    return;

  _z1->setWait(false);//unblock, check whether the trajectory has complete manually

  if(_lastCurTraj != _curTrajNum)
  {
    _z1->MoveJ( _trajectories[_curTrajNum].first, 
                _trajectories[_curTrajNum].second,
                joint_max_speed);
    _lastCurTraj = _curTrajNum;
  }else{
    if(_z1->_ctrlComp->recvState.state == UNITREE_ARM::ArmFSMState::JOINTCTRL) {
      if(++_curTrajNum > (_trajectories.size()-1)) {
        _curTrajNum = 0; // loop
      }
    }
  }
}

void TrajectoryManager::restart()
{
  _curTrajNum = 0; 
  _lastCurTraj = -1;
  running = false;
}