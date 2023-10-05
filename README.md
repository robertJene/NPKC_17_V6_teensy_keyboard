# NPKC_17_V6_teensy_keyboard
17-key keyboard with multiple layers

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

![20231002_075923](https://github.com/robertJene/NPKC_17_V6_teensy_keyboard/assets/131090265/1240cf41-d74f-4a10-8819-33cb100c97af)
![20231002_184533 (1)](https://github.com/robertJene/NPKC_17_V6_teensy_keyboard/assets/131090265/74008360-6a28-4b3e-b754-29f98f1d0f79)
![20231002_080040 (1)](https://github.com/robertJene/NPKC_17_V6_teensy_keyboard/assets/131090265/965cfdb8-d477-491b-a4db-3e764a06a842)
![20231002_075950](https://github.com/robertJene/NPKC_17_V6_teensy_keyboard/assets/131090265/1929578c-246a-43e2-b93e-f23dc343fa74)
