***************************
Chapter 2 - World Authoring
***************************

While Tsunagari doesn't yet include a comprehensive world editing suite, it is certainly possible to create an original game world. The process of manual world authoring isn't overly difficult, but requires a somewhat intimate understanding of some simple game data formats.

This chapter covers the procedures for creating a Tsunagari game world from scratch. It is intended for the tech-savvy user who doesn't mind getting their hands a bit dirty. Manual world authoring is not recommended for the common end-user; it requires a basic understanding of XML and other various file formats, and some manual file editing.

Anatomy of a World File
=======================

The Tsunagari Tile Engine is an interpreter for "world files", self-contained packages which consist of game data. World files themselves are simply zip archives with a ".world" file suffix. Inside these packages are several kinds of data, which together describe a game world. The different kinds of data files found inside a game world are described below.

World File Contents
-------------------

* World Descriptor: An XML file which must be named "world.conf", and must be present in the root directory of a world file. This file contains the base settings needed for the engine to initialize a game world.
* Player Descriptor: An XML file, of which exactly one must be present in a world file. This file contains initial settings for the player character, including movement speed, sprite data, and sound effects.
* NPC Descriptors: XML files, which contain the initial settings for a non-player-character, including movement speed, sprite data, and event script references for its AI.
* Audio Files: OGG Vorbis format audio files, with a ".oga" file suffix. These files contain sound effects and music.
* Tile/Sprite Sheets: PNG or JPEG format image files. Each of these files contain a sheet of tile graphics or sprite frames.
* Area Descriptors: XML files with a ".tmx" file suffix, which follow the TMX Map Format specification. Each of these files describe a single game world area. They are authored with the Tiled Map Editor, the use of which is described in a later chapter.
* Event Scripts: Python scripts which utilize the Tsunagari Event Scripting API to create dynamic game events. Event scripting is covered in a later chapter.

World Descriptor
================

The world descriptor is an XML file which must be named “world.conf” and must be present in the root directory of a world file. This is the first file that the Tsunagari Tile Engine looks for when loading a game world; it contains the configuration values needed to initialize the game. It includes settings such as information about the game, starting location for the player, initialization scripts, and input configuration.

Example World.conf (from Testing World)
---------------------------------------

::

   <?xml version="1.0" encoding="UTF-8" ?> 
      <world> 
      <info> 
         <name>Testing World</name> 
         <author>OmegaSDG</author> 
         <version>1</version> 
      </info> 
      <init> 
         <area>areas/grove01.tmx</area> 
         <player>entities/player/player.xml</player> 
         <mode>tile</mode> 
         <coords x="5" y="3" layer="0" /> 
         <viewport width="160" height="160" /> 
      </init> 
      <script> 
         <on_init>init.py</on_init> 
         <on_area_init>everyArea.py</on_area_init> 
      </script> 
      <input> 
         <persist init="300" cons="100" /> 
      </input> 
   </world>

That looks a little complicated. Let's break it down:

* The very first line is an instruction to the XML parser, which gives information about how the file should be processed. This line should be at the top of every XML file in the game world, and probably shouldn't be modified.
* The ``<world> </world>`` tags state that this file is a world descriptor. These are **required** at the beginning and end of world.conf.
* The ``<info> </info>`` tags denote the info section of world.conf. This section is **required**, and contains general information about the game world.

   * ``<name>Testing World</name>`` states that the name of the game world is "Testing World".
   * ``<author>OmegaSDG</author>`` states that this game world was written by "OmegaSDG".
   * ``<version>1</version>`` tells Tsunagari the version of the game world. This value isn't used yet, but is still required.

* The ``<init> </init>`` tags denote the init section of world.conf. This section is **required**, and contains information used for initializing the game.

   * ``<area>areas/grove01.tmx</area>`` gives the location of the starting area inside the world file. This area loads first, and the player is inserted here when the game starts.
   * ``<player>entities/player/player.xml</player>`` gives the location of the player entity descriptor, which is described in a later section.
   * ``<mode>tile</mode>`` states that this game is a "tile" mode game. Currently supported modes are "tile" and "turn". More game modes will be supported in the future. See the appendix for more information.
   * ``<coords x="5" y="3" layer="0" />`` states that the player will be inserted into the starting area at tile coordinates 5 by 3, on layer 0.
   * ``<viewport width="160" height="160" />`` states that the game window will scale to show at most 160 pixels of width and 160 pixels of height for each area. If the game's tiles are 16x16 pixels, then the window will show an area of 10 tiles by 10 tiles.

* The ``<script> </script>`` tags denote the script section of world.conf. This section is **optional**, and contains the locations of event scripts to be run at certain times.

   * ``<on_init>init.py</on_init>`` gives the location of an event script to be run when the game is first started.
   * ``<on_area_init>everyArea.py</on_area_init>`` gives the location of an event script to be run when each area is loaded.

* The ``<input> </input>`` tags denote the input section of world.conf. This section is **optional**, and contains input handling settings.

   * ``<persist init="300" cons="100" />`` states that in "turn" mode, when a direction button is held for 300 milliseconds, the player should continue to move in that direction every 100 milliseconds until the button is released. This value is required in "turn" mode only.

Tile and Sprite Sheets
======================

Tile and sprite sheets are PNG (or JPEG) image files containing either tile graphics or sprite frames. Both kinds of sheets follow the same format, and are in fact interchangeable, differing only in context. The sub-images in a sheet are referenced by their position in the sheet, starting at zero and counting left-to-right, then up-to-down. Sheets use the alpha channel instead of a predetermined background color to denote transparency.

As an example, this is the player sprite sheet for Tsunagari's testing world. The sheet contains twelve 16x18 pixel frames, which correspond to the player's direction and movement. It has been enlarged at %400 to increase clarity.

.. image:: _static/figure_sheet.png
   :align: center

To illustrate sheet positioning, here is the same sheet overlayed on a numbered chart to show sheet coordinates as they are referenced by the Tsunagari Tile Engine.

.. image:: _static/figure_sheet_numbered.png
   :align: center

Tile sheets are used to insert tile graphics in the Tiled Map Editor, and sprite sheets are used in entity descriptors when defining player and entity graphics phases.

Player Entity Descriptor
========================

NPC Entity Descriptors
======================

World File Packaging
====================

