#include "joystick/unitree_joystick.h"

namespace unitree_joy {

UnitreeJoystick::UnitreeJoystick(DogType dog)
  :Joystick("UnitreeJoystick"), _dog(dog)
{
  switch (_dog)
  {
  case DogType::Aliengo:
    _udp = std::make_shared<UNITREE_ARM::UDPPort>("192.168.123.220", 8012, 8112);
    _udp->resetIO(UNITREE_ARM::BlockYN::NO, UNITREE_LEGGED_SDK_ALIENGO::HIGH_STATE_LENGTH, 100000);
    break;
  case DogType::B1:
    _udp = std::make_shared<UNITREE_ARM::UDPPort>("192.168.123.220", 8012, 8112);
    _udp->resetIO(UNITREE_ARM::BlockYN::NO, UNITREE_LEGGED_SDK_B1::HIGH_STATE_LENGTH, 100000);
    break;
  }

  //create the connection
  _updateThread = std::make_shared< UNITREE_ARM::LoopFunc>("readJoystick", 0.01, boost::bind(&UnitreeJoystick::_read, this));
  _updateThread->start();
}

UnitreeJoystick::~UnitreeJoystick()
{
  _updateThread->shutdown();
}

void UnitreeJoystick::_read()
{
  int zerodata = 0;
  switch (_dog)
  {
  case DogType::Aliengo:
    _udp->send((uint8_t*)&_udpCmdAliengo, UNITREE_LEGGED_SDK_ALIENGO::HIGH_CMD_LENGTH);
    _udp->recv((uint8_t*)&_udpStateAliengo);
    memcpy(&_unitreeJoyData, _udpStateAliengo.wirelessRemote, 40);
    break;
  case DogType::B1:
    // Sneds single int data to sdk indicates that is not a command data, only for dog state
    _udp->send((uint8_t*)&zerodata, sizeof(int));
    _udp->recv((uint8_t*)&_udpStateB1);
    memcpy(&_unitreeJoyData, &_udpStateB1.wirelessRemote[0], 40);
    break;
  }
  _joyDataPre = _joyData;
  _convertJoyData(_unitreeJoyData, _joyData);
}

void UnitreeJoystick::_convertJoyData(xRockerBtnDataStruct& unitreeJoy, JoystickDataStruct& joy)
{
  //btn
  joy.btn.back  = unitreeJoy.btn.components.select;
  joy.btn.start = unitreeJoy.btn.components.start;
  joy.btn.LS    = 0;
  joy.btn.RS    = 0;
  joy.btn.LB    = unitreeJoy.btn.components.L1;
  joy.btn.RB    = unitreeJoy.btn.components.R1;
  joy.btn.A     = unitreeJoy.btn.components.A;
  joy.btn.B     = unitreeJoy.btn.components.B;
  joy.btn.X     = unitreeJoy.btn.components.X;
  joy.btn.Y     = unitreeJoy.btn.components.Y;
  joy.btn.up    = unitreeJoy.btn.components.up;
  joy.btn.down  = unitreeJoy.btn.components.down;
  joy.btn.left  = unitreeJoy.btn.components.left;
  joy.btn.right = unitreeJoy.btn.components.right;

  //axes
  joy.axes.lx = unitreeJoy.lx;
  joy.axes.ly = unitreeJoy.ly;
  joy.axes.rx = unitreeJoy.rx;
  joy.axes.ry = unitreeJoy.ry;
  joy.axes.LT = unitreeJoy.btn.components.L2;
  joy.axes.RT = unitreeJoy.btn.components.R2;
}

}