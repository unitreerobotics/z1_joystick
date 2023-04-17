#ifndef _ARM_CONTROLLER_
#define _ARM_CONTROLLER_

#include "unitree_arm_sdk/control/unitreeArm.h"
#include "joystick/joy_controller.h"

using namespace UNITREE_ARM;
using namespace unitree_joy;

/**
 * @brief A simple multi-segment trajectory manager
 * 
 * @warning only MoveJ is allowed
 */
class TrajectoryManager
{
public:
  TrajectoryManager(std::shared_ptr<unitreeArm> z1) : _z1(z1) {};
  ~TrajectoryManager(){};

  void addMoveJ(Vec6 endPosture, double gripper_angle);
  void clear() { _trajectories.clear();};
  void restart();
  void run();

  bool running = false;
private:
  const double joint_max_speed{2.0};
  int _curTrajNum{0}, _lastCurTraj{-1};
  std::vector<std::pair<Vec6, double>> _trajectories;

  std::shared_ptr<unitreeArm> _z1;
};

/**
 * @brief Maintains a controller, use joystick data to control z1.
 */
class ArmCtrl
{
public:
  ArmCtrl(std::string joyName);
  ~ArmCtrl(){};

private:
  // joint q to end posture
  Vec6 jointQ2posture(Vec6 q){
    return homoToPosture(z1->_ctrlComp->armModel->forwardKinematics(q));
  }

  void _run();

  void _jointCtrl();
  void _cartesianCtrl();
  void _trajectory(int trajNum);

  /**
   * @brief If z1 stay at State_JointCtrl for a long time, then start joint control
   * 
   * Maybe the controller(_currentState) haven't change to ArmFSMState::JOINTCTRL as expected.
   */
  void _checkForJointCtrl();

  std::shared_ptr<unitreeArm> z1;
  std::shared_ptr<JoyController> _joy;
  std::shared_ptr<LoopFunc> _runThread;
  std::shared_ptr<TrajectoryManager> _trajManager;

  ArmFSMState _currentState = ArmFSMState::INVALID;
  double _jointSpeed = 1.0;
  double _cartesianSpeed = 0.2;
  int cntJointState = 0;
};



#endif
