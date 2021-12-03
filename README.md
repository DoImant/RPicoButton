# Sample program for the RPicoButton class

This class is used for handling buttons on an RP2040 Raspberry Pico
up to 16 buttons can be queried simultaneously.
The GPIOs from 0 to 22 can be used with up to sixteen consecutive pins.
In addition to the query which buttons have been pressed, the duration of the press
is also determined and made available. The init method must be used to initialize the
pins to be used.

## The test setup for the example program presented

![Breadboard](https://github.com/DoImant/Stuff/blob/main/Taster-Debounce/Console-output.png?raw=true)

## Console output of the sample program

![Console](https://github.com/DoImant/Stuff/blob/main/Taster-Debounce/Taster-Debounce.png?raw=true)

## How to use the class headerfile in your code

* Include the headerfile rpicobutton.hpp in your sourcecode.

* Create an object of the data type RPicoButton.

RPicoButton checkButtons(\<pulluplogic [true,false]\>, \<internalResistors [true,false]\>, <debouncetime [ms]\>);

The expression RPicoButton "checkButtons(true, true, 200);" creates an object with pull-up logic,
the internal pull-up resistors are used and the debounce time is set to 200 ms.
  
The expression RPicoButton "checkButtons(false,false,100);" creates an object with pull-down logic,
external pull-down resistors are used and the debounce time is set to 100 ms.

After the object has been created, the button query must be initialized with the expression
"checkButtons.init(\<First GPIO Pin\>, \<Number of Pins\>);"

The first GPIO pin can be a value from 0 to 22. The number of pins used can be a maximum of 16.
If the basic GPIO plus the number of specified pins to be initialized results in a value higher than GPIO22,
initialization only takes place up to GPIO22 and the number of buttons that can be queried is reduced
accordingly.

## The methods of the class

In order to query the button(s), the **tic()** method must be called.

The **pressed()** method returns a value that corresponds to the bit pattern of the button(s) pressed. The value is
zero if no button has been pressed.

The **pressing_time()** method returns the time in ms that the button(s) were pressed.
