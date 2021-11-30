//////////////////////////////////////////////////////////////////////////////
/// @file rpicobutton.hpp
/// @author Kai R.
/// @brief Class for handling buttons. Up to 16 buttons can be queried simultaneously.
///        The GPIOs from 0 to 22 can be used with up to sixteen consecutive pins.
///        In addition to the query which buttons have been pressed, the duration of the press 
///        is also determined and made available. The init method must be used to initialize the 
///        pins to be used. 
///
/// @section How to use 
///
/// checkButtons(<pulluplogic [true,false]>, <internalResistors [true,false]>, <debouncetime [ms]>) 
/// 
/// The expression RPicoButton "checkButtons(true, true, 200);" creates an object with pull-up logic, 
/// the internal pull-up resistors are used and the debounce time is set to 200 ms.
///  
/// The expression RPicoButton "checkButtons(false,false,100);" creates an object with pull-down logic, 
/// external pull-down resistors are used and the debounce time is set to 100 ms.
///
/// After the object has been created, the button query must be initialized with the expression 
/// "checkButtons.init(<First GPIO Pin>, <Number of Pins>);".
///
/// The first GPIO pin can be a value from 0 to 22. The number of pins used can be a maximum of 16. 
/// If the basic GPIO plus the number of specified pins to be initialized results in a value higher than GPIO22, 
/// initialization only takes place up to GPIO22 and the number of buttons that can be queried is reduced 
/// accordingly.
///
///
/// @date 2021-11-26
/// @version 0.1
/// 
/// @copyright Copyright (c) 2021
/// 
//////////////////////////////////////////////////////////////////////////////

#ifndef _RPICOBUTTON_HPP_
#define _RPICOBUTTON_HPP_
#include <stdio.h>
#include "pico/stdlib.h"

#define MAX_GPIO      22
#define MAX_PINCOUNT  16

class RPicoButton {
  private:
    uint8_t  _basePin      {0};   // First pin of the input group
    uint8_t  _pinCount     {0};   // Number of input pins
    bool     _pullUp;             // Are the buttons connected as pull-up (true) or pull-down (false).
    bool     _internalResistors;  // If true then use the internal resistances of the controller.
    uint64_t _debTime;            // Debouncetim in µs
    uint16_t _state        {0};   // state of the GPIO-Pin Group
    uint16_t _maxState     {0};   // state when more than one GPIO goes up
    uint16_t _prevState    {0};   // previous state of GPIO-Pin Group
    uint16_t _lastState    {0};   // RPicoButton Bitmask after button(s) were released
    uint64_t _pressingTime {0};   // the length of time that the button was pressed (µs)
    uint16_t _inputPinmask {0};   // Bit mask of the GPIO input pins to which buttons are connected
    uint32_t _inDirMask    {0};   // Bit mask for setting the GPIO input pins
     
  public:
    // Constructor
    RPicoButton(bool pullUp = true, bool internalResistors = true, uint64_t debTime = 20) 
    : _pullUp {pullUp}, _internalResistors {internalResistors}, _debTime {debTime * 1000} {
    }

    void init(uint8_t, uint8_t);        // intialize the Pins
    void init(void);
    void tic(void);                     // mangage Inputs
    uint16_t pressed(void);             // returns the last Buttons state
    uint32_t pressing_time(void);       // returns how long the button(s) were pressed.
};

//////////////////////////////////////////////////////////////////////////////
/// @brief Initializes the pin group for the button query.
/// 
/// @param basePin            First GPIO pin of the group
/// @param pinCount           Number of GPIO pins to use
//////////////////////////////////////////////////////////////////////////////
void RPicoButton::init(uint8_t basePin, uint8_t pinCount) {
  _basePin = basePin;
  _pinCount = pinCount;
  // Check possible values      
  if (_basePin  > MAX_GPIO) _basePin = MAX_GPIO;
  if (_pinCount > MAX_PINCOUNT) _pinCount = MAX_PINCOUNT;
  if ((_basePin + _pinCount) > MAX_GPIO) _pinCount = (MAX_GPIO - _basePin) + 1;

  _inputPinmask = (1u<<_pinCount)-1;
  _inDirMask = _inputPinmask << _basePin;
 
  gpio_set_dir_in_masked(_inDirMask);
  if (_internalResistors) {
    for (uint8_t offset = 0; offset < _pinCount; ++offset) {
      if(_pullUp) {                          // RPicoButton at GND 
        gpio_pull_up(_basePin + offset);
      } else {                               // RPicoButton at VCC
        gpio_pull_down(_basePin + offset);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief  Determine which buttons were pressed and how long they were pressed.
/// 
//////////////////////////////////////////////////////////////////////////////
void RPicoButton::tic() {
  uint64_t now {(uint64_t)time_us_64()};
  _prevState = _state;
  // Get Input
  _state = ((_pullUp) ? ~(gpio_get_all() >> _basePin) : (gpio_get_all() >> _basePin)) & _inputPinmask;
  _state != _inputPinmask ? _state : 0;
  // Input done
  if (_state > _maxState) _maxState = _state;      // If several buttons have been pressed, note the maximum value

  if (_state && !_prevState) {                     // RPicoButton was pressed
    _pressingTime = now;
  } else if (!_state && _prevState) {              // RPicoButton was released
    _pressingTime = now - _pressingTime;
    if (_pressingTime >= _debTime) {               // Has the button been pressed longer than the debounce time?
       _lastState = _maxState;
    }
    _maxState = 0;
  } 
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Return of the determined bit pattern of the pressed keys.
///        After the status has been queried once, the status memory 
///        is set to zero.
/// 
/// @return uint16_t 
//////////////////////////////////////////////////////////////////////////////
uint16_t RPicoButton::pressed() {
  uint16_t tmp {_lastState};  
  _lastState = 0;       
  return tmp;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Return of the press duration in ms
/// 
/// @return uint32_t 
//////////////////////////////////////////////////////////////////////////////
uint32_t RPicoButton::pressing_time() {
  return _pressingTime / 1000;
}
#endif