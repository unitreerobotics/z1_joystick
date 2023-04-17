#ifndef _UNITREE_JOYSTICK_H
#define _UNITREE_JOYSTICK_H

#include "joystick/joystick.h"
#include "joystick/aliengo_common.h"
#include "joystick/b1_common.h"
#include "unitree_arm_sdk/control/unitreeArm.h"

namespace unitree_joy {

/**
 * @brief Describes the current dog type.
 * 
 * @attention The unitree_legged_sdk udp struct is different for different dogs.
 */
enum class DogType
{
  Aliengo,
  B1
};

/**
 * @brief Describes the unitree joystick keys
 */
typedef union {
  struct {
    uint8_t R1          :1;
    uint8_t L1          :1;
    uint8_t start       :1;
    uint8_t select      :1;
    uint8_t R2          :1;
    uint8_t L2          :1;
    uint8_t F1          :1;
    uint8_t F2          :1;
    uint8_t A           :1;
    uint8_t B           :1;
    uint8_t X           :1;
    uint8_t Y           :1;
    uint8_t up          :1;
    uint8_t right       :1;
    uint8_t down        :1;
    uint8_t left        :1;
  } components;
  uint16_t value;
} xKeySwitchUnion;

/**
 * @brief Describes the unitree joystick data
 */
typedef struct {
  uint8_t head[2];
  xKeySwitchUnion btn;
  float lx;
  float rx;
  float ry;
  float L2;// unused
  float ly;
  uint8_t idle[16];
} xRockerBtnDataStruct;

/**
 * @brief Maintains a connection to the unitree joystick, 
 * provides the current joystick data by unitree_legged_sdk
 */
class UnitreeJoystick : public Joystick
{
public:
  /**
   * @brief Construct a new Unitree Joystick object
   * 
   * @param dog Current unitree dog type
   */
  UnitreeJoystick(DogType dog);

  /**
   * @brief Closes the connection.
   */
  ~UnitreeJoystick();

private:
  void _read();
  void _convertJoyData(xRockerBtnDataStruct& unitreeJoy, JoystickDataStruct& joy);

  DogType _dog = DogType::B1;
  xRockerBtnDataStruct _unitreeJoyData{};
  std::shared_ptr<UNITREE_ARM::LoopFunc> _updateThread;
  std::shared_ptr<UNITREE_ARM::UDPPort> _udp;
  UNITREE_LEGGED_SDK_ALIENGO::HighCmd _udpCmdAliengo{};
  UNITREE_LEGGED_SDK_ALIENGO::HighState _udpStateAliengo{};
  UNITREE_LEGGED_SDK_B1::HighState _udpStateB1{};
};

}

#endif
