### Not working after July 16th. I won't update this repo anymore. If you still want to contact me: djnem23#1635

# altv-hack

Multihack for alt:V Multiplayer.

## Compile

Clone this repository using `git clone --recurse-submodules -j8 https://github.com/Keiichi32/altv-hack.git`

Navigate to the cloned folder and open `altv-hack.sln` with Visual Studio, and build it in Release/x64.

## Usage

Inject the DLL in the main menu after GTA5 loaded. You should use the **exec**, **remove** and **serial** commands before connecting to a server.

## Features

* JavaScript executor
* Client-side file dumper
* Hardware ID spoofer
* Server-side currentWeapon block

## Commands

* **exec [file name]** - Loads a file for executing code (put files in C:/altv-hack)
* **remove [string]** - Removes a string from all of the original scripts that are being loaded.
* **dump** - Dumps client-side files of current server (C:/altv-hack/dumper)
* **serial [number]** - Increments the hwid hashes by the given input, changes the SC Club nickname to a random string
* **weapon** - Blocks server-side getter for **currentWeapon** Player class property (use after spawning!)
