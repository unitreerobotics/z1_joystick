#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#include <stdint.h>
#include <string>

/**
 * @file joystick.h 
 * Contains the unitree_joy::Joystick type.
 */

namespace unitree_joy {

/**
 * @brief Decribes the joystick buttons.
 * 
 * All buttons occupy only one bit. The value is 1 when pressed.
 */
typedef struct
{
	uint8_t back        :1;
	uint8_t start       :1;
	/// Left anglog stick center pushed in
	uint8_t LS          :1;
	/// Right analog stick center pushed in
	uint8_t RS          :1;
	/// Left Bumper
	uint8_t LB          :1;
	/// Right Bumper
	uint8_t RB          :1;
	uint8_t A           :1;
	uint8_t B           :1;
	uint8_t X           :1;
	uint8_t Y           :1;
	uint8_t up          :1;
	uint8_t down        :1;
	uint8_t left        :1;
	uint8_t right       :1;
	/// reserved
	uint8_t idle1       :1;//保留
	/// reserved
	uint8_t idle2       :1;
}JoystickBtnSturct;

/**
 * @brief Describes the joystick axes.
 */
typedef struct 
{
	/**
	 * @brief The value of the left stick when moving horizontally
	 * 
	 * left : [1, -1] : right
	 */
	float lx;

	/**
	 * @brief The value of the left stick when moving vertically
	 * 
	 * up : [1, -1] : down
	 */
	float ly;

	/**
	 * @brief The value of the right stick when moving horizontally
	 * 
	 * left : [1, -1] : right
	 */
	float rx;

	/**
	 * @brief The value of the right stick when moving vertically
	 * 
	 * up : [1, -1] : down
	 */
	float ry;

	/**
	 * @brief The value of the left trigger when pressed
	 * 
	 * unpressed : [0, 1] : pressed
	 */
	float LT;

	/**
	 * @brief The value of the right trigger when pressed
	 * 
	 * unpressed : [0, 1] : pressed
	 */
	float RT;
}JoystickAxesSturct;

/**
 * @brief Describes the joystick data.
 */
typedef struct
{
	/**
	 * Current buttons data.
	 */
	JoystickBtnSturct btn{};

	/**
	 * Current axes data.
	 */
	JoystickAxesSturct axes{};
} JoystickDataStruct;

/**
 * @brief Maintains a Joystick base Class, provide the current joystick data.
 * 
 */
class Joystick
{
public:
	/**
	 * @brief Construct a new Joystick object
	 * 
	 * @param joyName Indicates the name of the joystick.
	 * Used to distinguish between different joystick types.
	 */
  Joystick(std::string joyName):_joyName(joyName) {};

	/**
	 * @brief Get the Joystick Name object
	 * 
	 * @return std::string 
	 */
	std::string getJoyName() {return _joyName;}

	/**
	 * @brief Get the current joystick data
	 * 
	 * @return JoystickDataStruct& 
	 */
  JoystickDataStruct &getdata() {return _joyData;};

	/**
	 * @brief Get the last joystick data
	 * 
	 * @return JoystickDataStruct& 
	 */
  JoystickDataStruct &getdataPre() {return _joyDataPre;};

protected:
  std::string _joyName="";
  JoystickDataStruct _joyData, _joyDataPre;
};

} // namespace unitree_joy

#endif
