#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#include <stdint.h>
#include <string>

/**
 * @file joystick.h 
 * Contains the unitree_joy::Joystick type.
 */

namespace unitree_joy {

class JoystickBtn 
{
public:
	JoystickBtn(){};

	/* Set button value */
	void operator()(const int& val) {
		this->dataPre_ = this->data_;
		this->data_ = val;
	}

	/* Get button value */
	int operator()() const {
		return this->data_;
	}

	bool isPressed() {
		return data_ == 1;
	}
	bool isJustPressed() {
		return (dataPre_ == 0) && (data_ == 1);
	}
	bool isJustReleased() {
		return (dataPre_ == 1) && (data_ == 0);
	}
private:
	int data_{}, dataPre_{};
};

class JoystickAxis{
public:
	JoystickAxis(){}

	/* Set axis value */
	void operator()(const float& val) {
		this->dataPre_ = this->data_;
		this->data_ = val;
	}

	/* Get axis value */
	float operator()() const {
		return this->data_;
	}

	bool isPressed() {
		return data_ > threhold_;
	}
	bool isJustPressed() {
		return (dataPre_ < threhold_) && ( data_ > threhold_);
	}
	bool isJustReleased() {
		return (dataPre_ > threhold_) && ( data_ < threhold_);
	}
private:
	float data_{}, dataPre_{};
	float threhold_ = 0.5; // consider an axis as a button
};

/**
 * @brief Decribes the joystick buttons.
 * 
 * All buttons occupy only one bit. The value is 1 when pressed.
 */
typedef struct
{
	JoystickBtn back;
	JoystickBtn start;
	JoystickBtn LS; /* Left anglog stick center pushed in */
	JoystickBtn RS; /* Right analog stick center pushed in */
	JoystickBtn LB; /* Left Bumper */
	JoystickBtn RB; /* Right Bumper */
	JoystickBtn A;
	JoystickBtn B;
	JoystickBtn X;
	JoystickBtn Y;
	JoystickBtn up;
	JoystickBtn down;
	JoystickBtn left;
	JoystickBtn right;
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
	JoystickAxis lx;

	/**
	 * @brief The value of the left stick when moving vertically
	 * 
	 * up : [1, -1] : down
	 */
	JoystickAxis ly;

	/**
	 * @brief The value of the right stick when moving horizontally
	 * 
	 * left : [1, -1] : right
	 */
	JoystickAxis rx;

	/**
	 * @brief The value of the right stick when moving vertically
	 * 
	 * up : [1, -1] : down
	 */
	JoystickAxis ry;

	/**
	 * @brief The value of the left trigger when pressed
	 * 
	 * unpressed : [0, 1] : pressed
	 */
	JoystickAxis LT;

	/**
	 * @brief The value of the right trigger when pressed
	 * 
	 * unpressed : [0, 1] : pressed
	 */
	JoystickAxis RT;
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
  Joystick(std::string joyName):joyName_(joyName) {};

	const std::string& getJoyName() const { return joyName_; }
  JoystickDataStruct &operator()() { return this->joyData_; };
	JoystickBtnSturct& btn() { return this->joyData_.btn; }	
	JoystickBtnSturct btn() const { return this->joyData_.btn; }	
	JoystickAxesSturct& axes() { return this->joyData_.axes; }	
	JoystickAxesSturct axes() const { return this->joyData_.axes; }	


protected:
  std::string joyName_="";
  JoystickDataStruct joyData_;
};

} // namespace unitree_joy

#endif
