********
Appendix
********

Movement Modes
==============

Tsunagari supports multiple styles of movement, called movement modes. These are set in the "mode" element of the "init" section in a game world's "world.conf" file. More information about world.conf is available in Chapter 2.

Currently supported movement modes are:

* ``turn`` : Turn-based mode, aka Roguelike mode. The player and NPC entities can only move one full tile at a time, and **do not** have by-pixel movement animations. NPC entities can only move at the same time that the player moves. Tile animations still function, and overlay entities can still move by-pixel.
* ``tile`` : Tile-based mode, aka Yume Nikki mode. The player and NPC entities can only move one full tile at a time, and **do** have by-pixel movement animations. NPC entities can move at any time.

Event Scripting API Reference
=============================

Source Code File Catalogue
==========================

