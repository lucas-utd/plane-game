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
- Use **vcpkg** as a submodule to get the same experience on all platforms
- Use **CMakePresets** to simplify the build process
- **CMakeUserPresets.json** for user-specific CMake configurations (Optional)

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
# Run those commands in Developer PowerShell for VS 2022
git clone --recurse-submodules -j8 https://github.com/lucas-utd/plane-game.git
cd plane-game

# Configure and build
cmake --workflow --preset x64-release
```

### 🔹 Linux (GCC + vcpkg)  
```sh
git clone --recurse-submodules -j8 https://github.com/lucas-utd/plane-game.git
cd plane-game

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


