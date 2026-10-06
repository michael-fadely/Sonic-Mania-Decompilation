# Building RSDKv5 & Sonic Mania for SEGA Dreamcast

This document will explain how to compile the Dreamcast toolchain, KallistiOS, and RSDKv5 + Sonic Mania (optionally, Plus!) for SEGA Dreamcast. You should be comfortable running commands in a terminal before continuing, *ideally* in a Linux environment. You're probably not going to have a good time otherwise!

Please also be aware that building the Dreamcast toolchain, KallistiOS, and Sonic Mania are all **intensive processes**; they will likely use a lot of CPU horsepower and RAM. There will be reminder warnings when the time comes.

Apologies in advance to developers who are decidedly not beginners in this area. This document isn't designed for you, but there are still important details below.

## 0: Requirements

- A copy of Sonic Mania (and Plus DLC if you want to legally play the DLC on DC) for game assets - easiest place is Steam
  - [Base game on Steam](https://store.steampowered.com/app/584400/Sonic_Mania/)
  - [Encore DLC on Steam](https://store.steampowered.com/app/845640/Sonic_Mania__Encore_DLC/) - **MUST** purchase to legally play Plus on Dreamcast
- Comfortable running commands in a terminal
- A Linux environment of some kind
  - **NOTE:** You may have trouble on immutable operating systems such as SteamOS and Bazzite!
  - [Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install) (called WSL going forward) is a valid option on Windows 10 and up. I use Ubuntu + WSL on Windows 11 without issue as my primary development platform.
    - **NOTE:** If you go this route, you might want to turn this off and uninstall your distro later! WSLv2 is a virtual machine and can sometimes use a lot of RAM and disk space.
  - MinGW/MSYS *may* work, but instructions for them will not be provided and they have not been tested with Mania/RSDK
- ffmpeg & ImageMagick for asset processing
  - Whatever your package repo has should be fine!

## 1: Setting up KallistiOS

Open up the following instructions and have them handy. We will be jumping back and forth. Both these instructions and the ones below will be executed in a terminal unless otherwise specified!
Setup instructions: https://dreamcast.wiki/Getting_Started_with_Dreamcast_development#Setting_up_and_compiling_the_toolchain_with_the_dc-chain_script

### 1.1: Toolchain

The toolchain is used for compiling Dreamcast code. We will need this for compiling KallistiOS *and* Sonic Mania.

Follow the Dreamcast wiki instructions for your platform **UNTIL THE SECTION "Cloning the KallistiOS git repository"**, then follow these instructions:
- Run `git clone https://github.com/KallistiOS/KallistiOS.git -b master /opt/toolchains/dc/kos`
- Run `cd /opt/toolchains/dc/kos/utils/kos-chain`
- Copy `Makefile.dreamcast.cfg` to `Makefile.cfg`
  - e.g. `cp Makefile.dreamcast.cfg Makefile.cfg`
- Edit `Makefile.cfg` with whatever tool you're comfortable with
  - `nano` is a great option for beginners, e.g. `nano Makefile.cfg`
  - In WSL, you can run Windows text editors. `notepad.exe Makefile.cfg` will open it up in Notepad
- Look for this line: `toolchain_profile=stable`
- Change it to: `toolchain_profile=14.4.0` - this version has been thoroughly tested with Mania
- Save your changes, and then, please note that...
- **THE NEXT STEP CAN TAKE A VERY LONG TIME, USE A LOT OF PROCESSING POWER, AND A LOT OF RAM! BE READY, AND GIVE IT TIME TO COMPLETE!**
- Run `make` to build the toolchain
- If everything works correctly, you can optionally run `make clean distclean` to free up some space

### 1.2: KallistiOS

This is the SDK that allows Mania/RSDK to interface with the Dreamcast.

- Run `cd /opt/toolchains/dc/kos`
- Copy `doc/environ.sh.sample` to `environ.sh`
  - e.g. `cp doc/environ.sh.sample environ.sh`
- Edit `environ.sh` in your editor of choice
  - Look for `export KOS_CFLAGS="${KOS_CFLAGS} -O2"` under the `# Optimization Level` section
  - Change `-O2` to `-Os`, so it looks like this: `export KOS_CFLAGS="${KOS_CFLAGS} -Os"`
- Save & close the file
- Run `source /opt/toolchains/dc/kos/environ.sh`
- Run `make`, and KallistiOS will be built

### 1.3: kos-ports / sh4zam

We'll be deviating from the Dreamcast wiki instructions for this one as we only need one of the available libraries in kos-ports: [sh4zam by Falco Girgis](https://github.com/gyrovorbis/sh4zam), a hardware-accelerated fast math library used for 3D rendering, i.e. in special stages.

**NOTE: IF YOU DID NOT JUST BUILD KallistiOS**, please make sure to run `source /opt/toolchains/dc/kos/environ.sh` before continuing.

- Run `git clone --recursive https://github.com/KallistiOS/kos-ports /opt/toolchains/dc/kos-ports`
- Run `cd /opt/toolchains/dc/kos-ports/sh4zam`
- Run `make install clean`

sh4zam should now be built and installed for use by Mania!

## 2: Building Sonic Mania & RSDK

Now it's time to grab and compile Sonic Mania and RSDK's code.

- Make a folder where you'd like to keep your code in whatever way you like. You will then need to open a terminal in that folder using the `cd` command. For example, `cd ~/mania-dreamcast`.
  - For WSL: This folder can be anywhere on the Windows host that is accessible by WSL (a storage device connected directly to your computer is ideal). For instance, if you created your folder at `C:\mania-dreamcast`, you can access it in WSL using the path `/mnt/c/mania-dreamcast`
- Run `git clone --recursive https://github.com/michael-fadely/Sonic-Mania-Decompilation.git` - this will create another folder called `Sonic-Mania-Decompilation`
- Run `cd Sonic-Mania-Decompilation`
  - For WSL: Make sure to open this folder in a WSL terminal, as we will need Linux going forward. e.g. `cd /mnt/c/mania-dreamcast/Sonic-Mania-Decompilation`
- Make a folder called `cmake-build`, e.g. `mkdir cmake-build`, then enter it with `cd cmake-build`
- **IF YOU HAVEN'T ALREADY, MAKE SURE TO RUN** `source /opt/toolchains/dc/kos/environ.sh` before continuing
- Next, run one of the following commands:
  - If you **DO NOT OWN the Encore DLC** or would like to disable it (**UNTESTED**):
    - `cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=/opt/toolchains/dc/kos/utils/cmake/kallistios.toolchain.cmake -DPLATFORM=KallistiOS -DGAME_STATIC=ON -DRETRO_DISABLE_PLUS=ON -DRETRO_REVISION=2 -DRSDK_DEBUG=OFF -DGAME_INCREMENTAL_BUILD=OFF -DKOS_USER_DIR=/cd/ ..`
  - **OR,** if you **DO OWN the Encore DLC**:
    - `cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=/opt/toolchains/dc/kos/utils/cmake/kallistios.toolchain.cmake -DPLATFORM=KallistiOS -DGAME_STATIC=ON -DRETRO_REVISION=2 -DRSDK_DEBUG=OFF -DGAME_INCREMENTAL_BUILD=OFF -DKOS_USER_DIR=/cd/ ..`
- **THE NEXT STEP CAN TAKE A VERY LONG TIME, USE A LOT OF PROCESSING POWER, AND A LOT OF RAM! BE READY, AND GIVE IT TIME TO COMPLETE!**
- Assuming no errors have occurred, you can now build the game by running `make`
- You should now have the file `cmake-build/dependencies/RSDKv5/RSDKv5.elf` - congrats! You have compiled the game.

## 3: Game asset prep

It's time to prep your files for creating a disc image!

### 3.1: Setting up the disc folder

This folder will hold all of the game assets necessary for the game to run. You can finish the following steps from your system's file browser.

- Create a new folder to store the files that will go in the disc image, i.e. `~/mania-dreamcast/disc` or `C:\mania-dreamcast\disc`
- Copy Sonic Mania's `Data.rsdk` into the disc folder. Details below for Steam:
  - In your Steam library, right click on Sonic Mania -> Manage -> Browse local files
  - You should see `Data.rsdk` - copy this to your disc folder
- Navigate to the Sonic Mania code folder from earlier, i.e. `~/mania-dreamcast/Sonic-Mania-Decompilation`
- Continue navigating to `dependencies/RSDKv5/dreamcast` and copy `mighty.ico` and `sonic.ico` to your disc folder
  - These icons will be used for the save files on the memory card

You should now have the following files in your disc folder:
- `Data.rsdk`
- `mighty.ico`
- `sonic.ico`

### 3.2: Asset processing

Now it's time to do asset processing. Returning to terminal territory! **NOTE** that this can take a **LONG TIME** depending on your hardware, and is **CRITICAL** for the game to function and perform properly.

- Open a terminal, and if you haven't already, install `ffmpeg` and `ImageMagick` for your platform
  - On Ubuntu, for example, this is as simple as `sudo apt install ffmpeg imagemagick`
- Point your terminal to `Sonic-Mania-Decompilation/dependencies/RSDKv5/dreamcast`
  - e.g. `cd "~/mania-dreamcast/Sonic-Mania-Decompilation/dependencies/RSDKv5/dreamcast"`
- **THE NEXT STEP CAN TAKE A VERY LONG TIME, USE A LOT OF PROCESSING POWER, AND A LOT OF RAM! BE READY, AND GIVE IT TIME TO COMPLETE!**
- Run the following command, making sure to change any paths that deviate from the examples given earlier:
  - `./generate_assets.sh "~/mania-dreamcast/disc/Data.rsdk" "~/mania-dreamcast/disc/Data" "~/mania-dreamcast/disc/Temp"`
- This should complete without errors!

Once completed, you should now have the following files/folder in your disc folder:
- `Data.rsdk`
- `mighty.ico`
- `sonic.ico`
- [folder] `Data`
  - [folder] `Data/Images`
  - [folder] `Data/Meshes`
  - [folder] `Data/Music`
  - [folder] `Data/SoundFX`
  - [folder] `Data/Sprites`
  - [folder] `Data/Video`

**NOTE:** If the `Temp` folder is still there, something likely went wrong! Stop and review the output in your terminal. **It is CRITICAL for performance and visual accuracy** that asset processing finishes correctly. **Your game may not work AT ALL if asset processing is incomplete!**

## 4: Creating a disc image

We will be using [mkdcdisc](https://gitlab.com/simulant/mkdcdisc) for this task using your Linux environment, and we will be creating a CDI file rather than a GDI in case you want to burn it to a disc. Burning the disc, or organizing for use on an ODE (such as GDEMU or TerraOnion MODE) will not be detailed here.

- Open a terminal and point it to your mania folder, e.g. `cd ~/mania-dreamcast`
- Follow [these instructions](https://gitlab.com/simulant/mkdcdisc/#building) for compiling
- Run the following command, making sure to change any paths that deviate from the examples given earlier:
  - `./builddir/mkdcdisc -e "~/mania-dreamcast/Sonic-Mania-Decompilation/cmake-build/dependencies/RSDKv5/RSDKv5.elf" -D "~/mania-dreamcast/disc" --title "MANIA" -o "mania.cdi" -N`
- If everything goes well, you should now have a file called `mania.cdi`

## 5: Play the game!
Enjoy!
