Introduction
===

This code is meant to be used with a [Teensy 2.0](http://www.pjrc.com/teensy/) in conjunction with momentary switch (e.g. [infogrip's Specs Switch](http://www.infogrip.com/products/switches/specs-switch.html)) to make a flexible, inexpensive button response system.

Currently it uses two 3.5mm TRS jacks that each have three contacts: tip, ring, and sleeve. The switch bridges either the tip or the ring to ground, triggering a keyboard command. If either of these is shorted to ground at boot, that pin is ignored.

For reference:
* tip - for typical stereo jacks this is the left channel; as wired this is yellow
* ring - for typical stereo jacks this is the right channel; as wired this is red
* sleeve - for typical stereo jacks this is ground; as wired this is black

Todo
===
* document the board wiring.
