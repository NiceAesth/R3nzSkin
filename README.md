<div align="center">

[![C++](https://img.shields.io/badge/Language-C%2B%2B-%23f34b7d.svg?style=plastic)](https://en.wikipedia.org/wiki/C%2B%2B)
[![LOL](https://img.shields.io/badge/Game-League%20of%20Legends-445fa5.svg?style=plastic)](https://na.leagueoflegends.com)
[![Windows](https://img.shields.io/badge/Platform-Windows-0078d7.svg?style=plastic)](https://en.wikipedia.org/wiki/Microsoft_Windows)
[![x64](https://img.shields.io/badge/Arch-x64-red.svg?style=plastic)](https://en.wikipedia.org/wiki/X86-64)
[![License](https://img.shields.io/github/license/NiceAesth/R3nzSkin.svg?style=plastic)](LICENSE)
[![Issues](https://img.shields.io/github/issues/NiceAesth/R3nzSkin.svg?style=plastic)](https://github.com/NiceAesth/R3nzSkin/issues)
[![MSBuild](https://github.com/NiceAesth/R3nzSkin/actions/workflows/msbuild.yml/badge.svg)](https://github.com/NiceAesth/R3nzSkin/actions/workflows/msbuild.yml)

# R3nzSkin

## Disclaimer

This is a fork of [R3nzSkin](https://github.com/R3nzTheCodeGOD/R3nzSkin) which I maintain for personal usage. You are free to use it but support is not guaranteed.

`R3nzSkin` is an internal skin changer for League of Legends.

</div>

- Change the skin of champions, your ward, towers, minions and jungle monsters during the game
- Automatic skin database updates
- Support for spectator mode
- Multi-language support
- In-game configuration menu
- Persistent configuration options

# Usage

1. Compile source or download from [releases](https://github.com/NiceAesth/R3nzSkin/releases/latest)
2. Use `R3nzSkin_Injector` or inject the resulting DLL into the game yourself
   - *Administrator* privilege may be needed if injecting fails
   - League client can crash if injected before going into arena
      - A workaround is to not inject until you are in the arena (you will need to be fast to not disrupt the game)
3. Press <kbd>Insert</kbd> to bring up the configuration menu

# AVX-2

This fork enables AVX-2 instructions. While this does increase performance, it reduces the supported CPUs.

Please check [this list](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#CPUs_with_AVX2) in order to see if your CPU is supported. Unless you are running an ancient system, you should have support.

# Credits
   The original version of [R3nzSkin](https://github.com/R3nzTheCodeGOD/R3nzSkin) is made by [R3nzTheCodeGOD](https://github.com/R3nzTheCodeGOD).
   
   R3nzSkin is an improved and updated version of the [B3akers](https://github.com/B3akers)/[LeagueSkinChanger](https://github.com/B3akers/LeagueSkinChanger) project.
