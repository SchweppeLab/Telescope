# Telescope
A lightweight database search algorithm and sandbox designed for developing and testing real-time search for mass spectrometry-based peptide identification.

# Getting Started
Telescope requires the [MSToolkit](https://github.com/mhoopmann/mstoolkit) and [NeoPepXMLParser](https://github.com/mhoopmann/NeoPepXMLParser).

* Telescope itself is developed in C++.
* There is a C# wrapper for use in C# application.
* TelescopeRTS is a data streamer application for testing RTS applications of Telescope. It also requires [Comet](https://github.com/UWPR/Comet).

Telescope from the command line requires a parameters file as the only input. A sample parameters file is provided with the software.
Example: >Telescope.exe telescope.params
