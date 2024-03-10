# FallingSandSimulation

A Falling Sand Simulation is a particle simulation game that illustrates dynamic interactions of various particles with simple rules such as sand, water, and fire within a 2D grid environment utilizing the principles of cellular automata. Players have the freedom to add or remove particles on the screen, with the simulation updating in real-time to display the resulting interactions.

## Core Components:
* **ParticleSimulator:** 
	- The simulation is designed to ensure order-independent operations. Traditional falling sand games, including the 'Noita', update particles from the bottom left to top right in a single buffer. This leads to interference and visible artifacts when multithreading and therefore depends largely on single threaded execution. By evaluating potential movements based on the previous frame and resolving multiple contenders for the same destination randomly, it ensures minimal artifacts even when computations are parallelized.
* **ParticleGrid:**
	- The Particle Grid manages all particle movements. It organizes the grid into chunks, each tracking "dirty rectangles" of particles requiring updates. These dirty rectangles are dynamically adjusted with every particle movement, creation, or deletion, optimizing the simulation's performance.
* **ParticleSystems:**
	- Each particle type is governed by its own system, which dictates how it updates based on specific rules. Particles exhibit unique states and behaviours, with a set of base rules applied to those moving below a certain velocity, ensuring a varied simulation.
* **Concurrency and optimizations:**
	- The simulation uses concurrent processing for various stages, including movement gathering, computation, and state simulation. However, particle density-based swaps are processed on a single thread to maintain visual consistency, as the order significantly influences the outcome.
* **UserInterface:**
	- The user interface allows for selecting particle types or tools. It also includes a debug mode for visualizing chunk boundaries and dirty rectangles, aiding in troubleshooting.
* **InputTools:**
	- **Stamp:** Allows for the creation or removal of particles in circular shapes, with adjustable size.
	- **Rectangle:** Enables filling, deleting, or moving particles within rectangular areas through mouse interactions.

## Dependencies and External Libraries:
* **SFML:**
	- Headers and libraries used for creating windows, handling events, and rendering the image.
* **Imgui:**
	- Used to display the User Interface.
* **stb_image.h:**
	- Used to decompress the tool image files for further use in Imgui.
* **Visual Studio:**
	- Project is set up to be developed with Microsoft Visual Studio.
