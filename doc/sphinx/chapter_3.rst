***********************
Chapter 3 - Map Editing
***********************

Rather than having its own map editor, Tsunagari uses "Tiled" for map creation and editing. Tiled is a program that allows you to easily create a map for use in a 2D game. It outputs TMX format map files, a format which Tsunagari is written to use.

Maps are the most important part of a game world. In order to create them, you'll need to grab a copy of Tiled from its website.

Tiled is not an OmegaSDG project.

`Download Tiled <http://www.mapeditor.org/>`_

Configuring Tiled
=================

.. image:: _static/tiled_01.png
   :scale: 25

This is Tiled. It looks very simple because it is very simple. In this chapter, the features and usage of Tiled will be documented by breaking down the recreation of a few areas that show up in the Tsunagari testing world.

First, Tiled needs to be configured to produce files that Tsunagari can understand. Navigate to ``Edit > Preferences`` in the topbar menu.

.. image:: _static/tiled_02.png

The options that need to be changed are under the "General" tab. In the newest version of Tiled, the required settings are as follows:

* Store tile data as: XML
* Include DTD reference in saved maps: OFF

Once the Preferences dialog is closed, Tiled should remember these settings.

Starting a New Map (Area)
=========================

After modifying Tiled's preferences, the next thing to do is create a new map. A map is a file that describes an "area" in Tsunagari. Each one represents a single area of the game world.

To create a new map, navigate to ``File > New`` in the topbar menu.

.. image:: _static/tiled_03.png

Make sure that "Orientation" is set to "Orthogonal", and that the "Tile Size" settings match with the size of the tiles in your tile sheets. "Map Size" is exactly what it sounds like; it sets the width and height of the new map in tiles.

.. image:: _static/tiled_04.png
   :scale: 25

Setting Map Properties
----------------------

Next we need to set some properties that Tsunagari will require in order to prepare the area for playing when it's loaded. "Properties" are data values left inside the TMX file for Tsunagari to read. Almost anything that can be modified with Tiled can have properties.

To set the map's properties, navigate to ``Map > Map Properties``.

.. image:: _static/tiled_05.png

To add a new property, double click on "<new property>" under the name field. Once a property is added, you can set its value in the value field. The properties used in Tsunagari maps are as follows:

* "name": (**REQUIRED**) The name of the map area.
* "author": (**REQUIRED**) The name of the map's creator.
* "intro_music": (**OPTIONAL**) Path to a music file played exactly once when the area is entered.
* "main_music": (**OPTIONAL**) Path to a music file played continuously. Played after intro_music if it exists.
* "on_load": (**OPTIONAL**) Trigger for an event script or function to be run when the area first loads.
* "on_focus": (**OPTIONAL**) Trigger for an event script or function to be run each time the area is entered.
* "on_tick": (**OPTIONAL**) Trigger for an event script or function to be run every frame.
* "on_turn": (**OPTIONAL**) Trigger for an event script or function to be run every time the player moves in TURN mode. Different game modes are discussed later.
* "loop": (**OPTIONAL**) The looping area setting. Use "x" for a horizontal loop, "y" for a vertical loop, or "xy" for a full loop.
* "color_overlay": (**OPTIONAL**) Overlay a color onto the entire screen inside this area. Uses format "r,g,b,alpha".

Event script triggers always use one of two formats:

* "path/to/script.py": Run the specified script when the trigger is activated.
* "path/to/script.py:function()": Run the specified script and the specified function inside it when the trigger is activated. No arguments are accepted.

Custom map properties can also be set, and then read later with event scripting.

Tile Sheets
===========

Before you can place any tiles on your map, you have to link in a tile sheet. Tile sheets contain graphics for the tiles to be used in a map; they're described in more detail in the section on sprite and tile sheets in Chapter 2.

To link a tile sheet, navigate to ``Map > New Tileset`` in the topbar menu.

.. image:: _static/tiled_06.png

The "Name" field should contain the name of the tile sheet's image file, and the "Image" field should contain the file's path, relative to the top directory of your world. Absolute directories relative to your filesystem will not work.

The "Tile width" and "Tile height" fields should contain the width and height of the tiles in your tile sheet, in pixels.

The other options in this dialog are for specialized uses, and won't be discussed here. They are mostly self-explanatory, if needed.

.. image:: _static/tiled_07.png
   :scale: 25

Once a tile sheet is linked in, its contents will appear in the "Tilesets" dialog in the bottom right of Tiled's window. This dialog is used for placing tiles and setting global tile properties.

Global tile properties are properties that apply to every instance of that tile which is placed on the map. Tile properties in general are discussed across several later sections.

Animations
==========

Layers
======

Placing Tiles
=============

Placing Objects
===============

Tile Flags
==========

Event Triggers
==============

Layermod
========

Lazy Layermods
--------------

Linking Areas with Exits
========================

Lazy Exits
----------

Wide Exits
----------

