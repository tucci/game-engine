# game-engine
A place for me to experiment/build an entire game engine from scratch


# Features
- PBR Rendering wth IBL(HDR Skyboxes)
- ECS using Data Oriented Design
- Scene Hierarchy Management
- Asset System
- FBX importing
- Logging
- Editor Integration with IMGUI
- Undo/Redo System
- Early Experimental Multi Threaded Job System


# Editor features
- Scene Viewport for real time editing
- Game Viewport to see game run in editor
- Multi Viewports (Top, Side, Front Views)
- Inspector with all the component data
- Asset Browser with drag and drop
- Scene Hierarchy with drag and drop parenting
- Log with filtering 
- Render System Data
- Game Loop/Engine timer Data

# Libs
- zlib
- sdl
- imgui

# Philosophy for writing this engine
- No libraries such as stl and boost. (No std::vector or std::string)
- Minimize dependencies
- Try to write everything from scratch as much as possible
- Program using Data Oriented Design
- Avoid OOP
- Program in C++, but avoid all the crazy modern C++ insanity
	- Templates are used sparingly (Only containers), also to avoid bloated compile times
	- Minimal use of operator overloading. Only used for Vectors, Matrices, and Quaternions
	- No Virtuals, Inheritance, or OOP-ness
	- No new/delete. Try to use custom memory allocators for everything
	- Avoid autos. Prefer fixed width int types (uint32_t/u32 over int)
	- No C++ Exceptions
	- No RTTI
	- Try to think hard about object lifetime and ownership.
- Avoid too many layers of abstraction
- Be explicit rather than implicit. Code is easier to follow in the debugger
- Think about the data and data flow
- Be wary about memory latency, memory layout, and the memory hierarchy
- Understand that you are writing code for a real machine with constraints, finite resources and not some abstract machine
	- How much memory does a system need?
	- How much time does a system need to perform its task?
	- Does this code need to be done now, or can it be done later?
- Write code that I need now and not in the future
- Write code that is easy to debug, understand, and easy to change
- Write code that can be multi-threaded easily
- Try to read the outputted assembly and try to understand what it is trying to do
- Understand that the compiler can't do everything


# Screenshots
![Screenshot 1](https://raw.githubusercontent.com/tucci/game-engine/master/Screenshots/Engine_8UYXxVbiNY.png)
![Screenshot 2](https://raw.githubusercontent.com/tucci/game-engine/master/Screenshots/Engine_FDoARnnZ1M.png)
![Screenshot 3](https://raw.githubusercontent.com/tucci/game-engine/master/Screenshots/Engine_YPesTn9LkU.png)
![Screenshot 4](https://raw.githubusercontent.com/tucci/game-engine/master/Screenshots/Engine_w2hFGXEGDH.png)
