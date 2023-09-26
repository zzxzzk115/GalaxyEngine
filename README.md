# Galaxy

<!-- TODO: Add Logo Icon -->
<!-- <div align="center">
    <img src="./Docs/Media/Logo.svg" />
</div> -->
<h4 align="center">
  <strong>🚀Galaxy</strong> is a cross-platform 3D game engine.
</h4>


<p align="center">
    <a href="https://github.com/zzxzzk115/GalaxyEngine/actions" alt="CI-Windows">
        <img src="https://img.shields.io/github/actions/workflow/status/zzxzzk115/GalaxyEngine/BuildWindows.yml?branch=master&label=CI-Windows&logo=github" /></a>
    <a href="https://github.com/zzxzzk115/GalaxyEngine/actions" alt="CI-Linux">
        <img src="https://img.shields.io/github/actions/workflow/status/zzxzzk115/GalaxyEngine/BuildLinux.yml?branch=master&label=CI-Linux&logo=github" /></a>
    <a href="https://github.com/zzxzzk115/GalaxyEngine/actions" alt="CI-MacOS">
        <img src="https://img.shields.io/github/actions/workflow/status/zzxzzk115/GalaxyEngine/BuildMacOS.yml?branch=master&label=CI-MacOS&logo=github" /></a>
    <a href="https://github.com/zzxzzk115/GalaxyEngine/issues" alt="GitHub Issues">
        <img src="https://img.shields.io/github/issues/zzxzzk115/GalaxyEngine">
    </a>
    <a href="https://github.com/zzxzzk115/GalaxyEngine/blob/master/LICENSE" alt="GitHub">
        <img src="https://img.shields.io/github/license/zzxzzk115/GalaxyEngine">
    </a>
</p>

> Galaxy is still under development from scratch.

## Prerequisites

To build **🚀Galaxy**, you must first install the following tools.

### Windows 10/11

- Visual Studio 2019 (or above) with C++ Desktop Development Environment
- CMake 3.20 (or above)
- Git 2.1 (or above)
- Vulkan SDK 1.3.261.1 (or above)

### Ubuntu 22.04 or above (Linux or WSL2)

Install prerequisite packages:

```bash
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
sudo apt-get update
sudo apt-get install build-essential cmake git clang libx11-dev libxrandr-dev libxrender-dev libglvnd-dev libxinerama-dev libxcursor-dev libxi-dev mesa-vulkan-drivers vulkan-sdk
```

### MacOS

- XCode 13 (or above)
- CMake 3.20 (or above)
- Git 2.1 (or above)
- Vulkan SDK 1.3.261.1 (or above)

## Build Galaxy

Clone this repository:

```bash
git clone --recursive git@github.com:zzxzzk115/GalaxyEngine.git
```

### Build on Windows

Build it automatically:

Just double-click `BuildWindows.bat ` or you can simply use Jetbrains CLion to debug or release.

Build it manually:

```bash
cmake -S . -B build
```

Then open the generated solution file with Visual Studio.

### Build on Linux / WSL2

Build it automatically:

```bash
chmod +x *.sh && ./BuildLinux.sh
```

### Build on MacOS

Build it automatically:

```bash
chmod +x *.sh && ./BuildMacOS.sh
```

Build it manually:

```bash
cmake -S . -B build -G "Xcode"
```

Then open the generated project file with XCode.

## Acknowledgments

We would like to thank the following projects for their invaluable contribution to our work:

- [Hazel](https://github.com/TheCherno/Hazel) (Game Engine & [Tutorials](https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT))
- [Piccolo](https://github.com/BoomingTech/Piccolo) (Game Engine & [Tutorials](https://space.bilibili.com/512313464/channel/collectiondetail?sid=292637))
- [GameEngineFromScratch](https://github.com/netwarm007/GameEngineFromScratch) (Game Engine & [Tutorials](https://zhuanlan.zhihu.com/c_119702958))
- [gamemachine](https://github.com/Froser/gamemachine) (Game Engine & [Tutorials](https://www.zhihu.com/column/c_1033300557322121216))
- [AsunaEngine](https://github.com/xiaoshichang/AsunaEngine) (Game Engine & [Tutorials](https://www.zhihu.com/column/c_1352653422265643008))
- [levk](https://github.com/karnkaul/levk) (Game Engine)
- [FlaxEngine](https://github.com/FlaxEngine/FlaxEngine) (Game Engine)

## License

This project is licensed under the [GPLv3](https://github.com/GalaxyEngine/Galaxy/blob/master/LICENSE) license.
