************************
Chapter 1 - Engine Usage
************************

The Tsunagari Tile Engine is an interpreter for Tsunagari game world files. Each game world is a self-contained package, which can be run and played by the correct version of the engine. During Alpha and Beta development stages, world files should not be expected to work across multiple versions of the engine, since features are being constantly added, removed, and changed.

This chapter covers standard usage of the Tsunagari Tile Engine for the common end-user; it explains how to use the engine to play a pre-built game world, and the options and settings which can be modified to enhance the gameplay experience. World editing and advanced topics are covered in the next chapters.

Client Config File
==================

A file named "client.ini" is usually present in the same directory as the Tsunagari Tile Engine executable. This file contains the engine's default gameplay and configuration settings, in a layman-friendly format. The file is split into sections, with each section containing modifiable options which affect the operation of the engine.

When the engine is run, this file is read and its settings are used as defaults, which may be overridden by command line options, described in the next section. If "client.ini" is missing, the engine will fall back on compiled-in defaults.

Example Client.ini
------------------

::

   [engine]
   world = testing.world
   datapath = data1.zip,data2.zip
   verbosity = verbose
   scripthalt = false

   [window]
   width = 320
   height = 320
   fullscreen = false

   [audio]
   enabled = true

   [cache]
   enabled = true
   ttl = 300
   size = 100

The above settings and their effects are described below:

* [engine] Section

   * "world": This option sets the filename of the game world to be loaded and played by the engine at runtime.
   * "datapath": A comma delimited list of zip files to be prepended to the engine's data path. See section "Data Path" in chapter 5.
   * "verbosity": This option sets the types of messages to be sent to the console. It has no noticeable effect unless the engine is run from the command line. It accepts the following values:

      * "normal": Only error messages are sent to the console. Error type messages generally describe game world design oversights, or the cause of a sudden crash.
      * "verbose": Debug messages are sent to the console. Debug type messages generally describe information which would be useful to a game world designer in the process of building a game, or to a developer working on the engine itself. This produces large amounts of output.
      * "quiet": Only fatal error messages are sent to the console. Fatal errors describe the cause of an engine crash.

   * "scripthalt": This option sets whether the engine should quit when an event script encounters an error.

* [window] Section

   * "width": This option sets the width of the window, or the width of the view area in fullscreen.
   * "height": This option sets the height of the window, or the height of the view area in fullscreen.
   * "fullscreen": This option sets whether the engine should run fullscreen or in a window. It accepts the following values:

      * "true": Run in fullscreen mode.
      * "false": Run in a window.

* [audio] Section

   * "enabled": This option sets whether sound effects and music are enabled or disabled. It accepts the following values:

      * "true": Enable all audio.
      * "false": Disable all audio.

* [cache] Section

   * "enabled": This option sets whether the resource cache should be used. Disabling the cache will negatively effect engine performance in most or all cases. It accepts the following values:

      * "true": Use the resource cache.
      * "false": Do not use the resource cache. (May increase CPU usage.)

   * "ttl": The resource cache's "time-to-live" in seconds, or the amount of time each resource is cached following disuse. Lowering this value may increase performance on computers with little RAM.
   * "size": The maximum size of the resource cache, in megabytes. This translates directly into RAM usage; it can be increased to improve engine performance, or decreased to conserve memory.

Command Line Options
====================

While the client config file is used for setting defaults, one-time options may be set from the command line. Command line options are used to temporarily override the settings in the config file, for purposes such as testing an alternative setting or trying out a new game world. Command line options may be used from a script (not discussed here), or from your operating system's command line interface.

The world file to be run can be specified on the command line by giving its path as an argument.

If Tsunagari is run from the command line with no options, it simply uses the client config file. Note that engine messages can only be viewed when the engine is run from the command line.

* ``Usage: ./tsunagari [OPTIONS] [WORLD FILE]``

Command line options and the client config file options they override are described below:

* ``-h/--help``: Display a help message containing a summary of command line options.
* ``-c/--config <config file>``: Read an alternative client config file.
* ``-p/--datapath <file,file,...>``: Prepend zips to data path. See section "Data Path" in chapter 5.
* ``-q/--quiet``: Display only fatal errors in the console.
* ``--normal``: Display all errors in the console.
* ``-v/--verbose``: Display additional information in the console.
* ``-t/--cache-ttl <seconds>``: Override [cache] "ttl". (Set resource cache time-to-live in seconds.) A value of zero disables the cache.
* ``-m/--cache-size <megabytes>``: Override [cache] "size". (Set resource cache size in megabytes.) A value of zero disables the cache.
* ``-s/--size <WxH>``: Override [window] "width" and [window] "height". (Set width x height of the window or view area.)
* ``-f/--fullscreen``: Override [window] "fullscreen". (Run in fullscreen mode.)
* ``-w/--window``: Override [window] "fullscreen". (Run in a window.)
* ``--script-halt``: Override [engine] "scripthalt". (Engine will stop on event script errors.)
* ``--no-audio``: Override [audio] "enabled". (Disable sound effects and music.)
* ``--query``: Query compiled-in engine defaults.
* ``--version``: Show the engine's version.

Gameplay Input
==============

Currently, there are several hardcoded keyboard input events which have the same function across all games.

* Escape: Pause the engine and display the pause screen.
* Shift + Escape: Exit the engine.
* Arrow Keys: Move in the specified direction.
* Ctrl + Arrow Keys: Look in the specified direction.
* Space: Trigger the use event on an object or tile.

