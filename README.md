# Depth Wizardry

An interactive Unreal Engine 5 pipeline designed for monocular depth estimation, satellite imagery elevation processing, and real-time 3D terrain displacement.

---

## Overview

Depth Wizardry bridges computational depth estimation with real-time game engine rendering. The system processes 2D satellite and orthographic imagery, translates estimated depth maps into elevation metrics via a custom C++ subsystem, and dynamically reconstructs 3D terrain using displacement shaders in Unreal Engine 5.

### Key Highlights
- **Elevation Subsystem (`ElevationSubsystem.cpp`):** Custom engine subsystem handling height data conversion, coordinate projection, and runtime level queries.
- **Dynamic Terrain Displacement:** Custom shaders (`M_TerrainDisplacement`) rendering real-time mesh height displacement from depth maps.
- **Real-Time Visualization:** Interactive first-person perspective navigation across reconstructed geographic terrain.

---

## Tech Stack
- **Engine:** Unreal Engine 5 (UE5)
- **Core Logic:** C++ (Subsystems, GameMode, Player Controller)
- **Shaders & Rendering:** Material Graph, World Position Offset / Dynamic Displacement
- **Visuals / Framework:** Enhanced Input, FirstPerson Template Integration

---

## Repository Structure

```text
depth-wizardry/
├── Config/               # Project and engine default configurations (.ini)
├── Content/              # Maps, displacement materials, and player Blueprints
│   ├── satellite_imagery/ # 3D terrain maps and displacement master materials
│   └── FirstPerson/      # Character controller and game mode setup
├── Source/               # Custom C++ engine modules and build targets
│   └── satillite_imagery/# ElevationSubsystem, PlayerController, and GameMode
└── satillite_imagery.uproject # Main project descriptor

Prerequisites
Unreal Engine 5.3+

Visual Studio 2022 (with Game development with C++ and Unreal Engine IDE Support installed)

Setup & Launch
Clone the repository:

Bash
git clone [https://github.com/agutthamkumar-design/depth-wizardry.git](https://github.com/agutthamkumar-design/depth-wizardry.git)
cd depth-wizardry
Generate project files:

Right-click satillite_imagery.uproject and select Generate Visual Studio project files.

Compile and run:

Open satillite_imagery.sln in Visual Studio 2022.

Set configuration to Development Editor and build the solution (Ctrl + Shift + B).

Launch the project directly from Visual Studio or double-click satillite_imagery.uproject.

Open the terrain map:

In the Content Drawer, navigate to Content/satellite_imagery/3d_map.umap and press Play (PIE).

Authors & Contributors
Uttham Kumar
