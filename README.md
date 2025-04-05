[![Build](https://github.com/lucas-utd/plane-game/actions/workflows/cmake-vcpkg-build.yml/badge.svg)](https://github.com/lucas-utd/plane-game/actions/workflows/cmake-vcpkg-build.yml)

# 🚀 Plane Game (C++/SFML)

## ✈️ Overview  
This is a cross-platform 2D plane game built with **C++** and **SFML**. The project is designed with **modern C++ features** (such as concepts in template programming) and follows **software design patterns** to ensure maintainability and scalability.  

## 🎯 Features  
- Cross-platform support (**Windows & Linux**)  
- Uses **CMake** for project configuration  
- Dependency management via **vcpkg**  
- Leverages **modern C++** (C++20 concepts, smart pointers, and more)  
- Implements **software design patterns** for better code organization  
- Engaging **SFML-powered** gameplay with smooth graphics  
- **GitHub Workflow** for automatic testing on **Windows** and **Ubuntu**  

## 🛠️ Build Instructions  

### 🔹 Prerequisites  
Ensure you have the following installed:  
- **CMake** (≥3.28)  
- **SFML** (via vcpkg)  
- **C++ compiler** (MSVC, GCC, or Clang)  
- **vcpkg** for dependency management  
- **CMakePresets.json** for the default CMake configurePresets (Optional)
- **CMakeUserPresetsTemplate.json** for the CMake workflow configurePresets (Optional)

### 🔹 Windows (MSVC + vcpkg)  
```PowerShell
git clone https://github.com/lucas-utd/plane-game.git
cd plane-game

# Install dependencies via vcpkg
if (!(Test-Path "C:\vcpkg")) {
    git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
    cd c:\vcpkg
    bootstrap-vcpkg.bat
}

# Configure and build
cmake -S . -B build -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

### 🔹 Linux (GCC + vcpkg)  
```sh
git clone https://github.com/lucas-utd/plane-game.git
cd plane-game

# Install dependencies via vcpkg
if [ ! -d "~/vcpkg" ]; then
    echo "VCPKG not found, installing..."
    git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
    cd ~/vcpkg
    ./bootstrap-vcpkg.sh
fi

# Configure and build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

### 🔹 Use CMake WorkFlow (Optional)
1. Rename the CMakeUserPresetsTemplate.json file to CMakeUserPresets.json
2. Add value for VCPKG_ROOT Environment variable in the vc-debug configurePresets ("C:\vcpkg" for Window, "~/vcpkg" for Linux)

```sh
# Configure and build
cmake --workflow --preset x64-release
```

## 🎮 How to Play  
- **Controls:**  
  - Arrow keys: Move the plane  
  - Spacebar: Fire  
  - Escape: Pause/Exit  

- **Objective:**  
  - Avoid enemies and obstacles  
  - Destroy enemy planes to score points  

## 🏗️ Code Structure  
- **`src/`** - Game source files  
- **`include/`** - Header files  
- **`Media/`** - Game assets (images, sounds, etc.)  
- **`CMakeLists.txt`** - Build configuration  
- **`CMakePresets.json`** - CMake Preset  
- **`.github/workflows/`** - GitHub Actions workflow files  

## 🔄 Continuous Integration (CI)  
This project includes a **GitHub Actions workflow** that automatically builds and tests the game on **Windows** and **Ubuntu**. The workflow ensures that all commits and pull requests maintain code quality and compatibility across platforms.  

## 📌 Key Technologies  
- **C++20** (Concepts, smart pointers, structured bindings)  
- **SFML** (Graphics, Audio, Input handling)  
- **CMake** (Cross-platform build system)  
- **vcpkg** (Dependency management)  
- **Software Design Patterns** (Singleton, Factory, Observer, etc.)  
- **GitHub Actions** (Automated CI testing)  

## 🤝 Contributing  
Contributions are welcome! If you’d like to improve the game, follow these steps:  
1. Fork the repository  
2. Create a new branch (`git checkout -b feature-xyz`)  
3. Commit your changes (`git commit -m "Add feature XYZ"`)  
4. Push the branch (`git push origin feature-xyz`)  
5. Open a **Pull Request**  

## 📜 License  
This project is licensed under the **MIT License** – feel free to use and modify it!  


