# CubeX - An Internal Hack for AssaultCube v1.3.0.2
<img width="1796" height="971" alt="image" src="https://github.com/user-attachments/assets/1252d6c6-d9de-4c85-b9cf-1feb63d14085" />

## Features

- **ESP**: See players through walls with boxes, health bars, and names
- **Aim Assistance**: Automatically aim at enemies with customizable smoothing
- **Menu** Easy-to-use interface to configure all settings

## Building
1. Install dependencies with vcpkg:
   ```bash
   vcpkg install detours:x86-windows imgui:x86-windows
2. Open `cubex.sln` in Visual Studio

3. Build the project (Make sure you're building for **Release x86**)

4. The DLL will be in the `build/` folder

## Usage
1. Inject `cubex.dll into AssaultCube
2. Press `INSERT` to open the menu
3. Configure your settings

## Disclaimer
This is for educational purposes only. Using trainers in online games may violate terms of service and result in bans. Use responsibly.
**No responsibility is claimed for any consequences resulting from the use of this software. Use at your own risk.**
