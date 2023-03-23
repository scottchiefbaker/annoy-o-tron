# USB Annoy-o-tron

Turn your ESP32-S2 into an annoying mouse moving and keyboard pressing "toy".

## What is it?

ThinkGeek used to make a ["Phantom Keystroker"][1] that would randomly input
keys and mouse movements as a prank. Sadly the device is no longer being made.
Enter the ESP32-S2 with it's USB emulation mode! Now we can recreate the fun
of the original toy on a $2 micro-controller.

## Why would I use it?

To mildly inconvenience your friends and enemies, or to play a practical
joke on your co-workers.

## How does it work?

Annoy-o-tron uses the built-in USB-OTG capabilities of your ESP32-S2 to
emulate a keyboard and a mouse. Every ten seconds a random event
will fire. Potential events and their probabilities of firing are:

| Event                       | Probability |
| --------------------------- | ----------- |
| Random mouse movement       | 1 in 10     |
| Random arrow key            | 1 in 50     |
| Right click mouse           | 1 in 100    |
| Send `ALT` + `TAB`          | 1 in 100    |
| Toggle capslock on/off      | 1 in 200    |
| Send `CTRL` + `ALT` + `DEL` | 1 in 500    |
| Lock screen (`WIN` + `L`)   | 1 in 1000   |

## Installation

Download the Annoy-o-tron sketch, compile it on to your ESP32-S2, and then
check the serial output to see events firing. You can tweak the probabilities
to your liking in the code. Once you are happy with how it's performing,
you can sneak it behind your victim's machine and watch from a safe distance.

[1]: https://www.amazon.com/ThinkGeek-Phantom-Keystroker-High-Tech-Office-Based/dp/B002YJW7C4/
