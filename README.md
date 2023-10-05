# NPKC_17_V6_teensy_keyboard
17-key keyboard with multiple layers

Go here for pictures:
https://github.com/robertJene/NPKC_17_V6_teensy_keyboard/issues/1

This is provided as an example, please see the license file.

Purpose:
a 17-key keypad made from a keyboard switch tester and a Teensy 3.2

it has these layers:

layer 1(0): keypad keys
layer 2(1): with normal number keys that don't use keypad scancodes
layer 3(2): with gaming WASD keys and surrounding keys
layer 4(3): a section to put in macros
layer 5(4): media keys

You press and hold the top left button, which activates the function
then press the 2nd button to go to the next layer, the 3rd to go to the previous layer, or the 4th which will type out information about the layer

I programmed in 10 layers, most of them are empty.
The LED's change depending on what layer you are on.
LED's appear behind the buttons for each layer like so:

layer 1(0)
LEDS: O - - -

Layer 2(1)
LEDS: - O- - -

Layer 3(2)
LEDS: - - O -

Layer 4(3)
LEDS: - - - O

Layer 5(4)
LEDS: O O - -

Layer 6(5)
LEDS: O - O -

Layer 7(6)
LEDS: O - - O

Layer 8(7)
LEDS: - O O -

Layer 9(8)
LEDS: - O - O

Layer 10(9)
LEDS: - - O O


