# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/arthur-kamau/Software/clion-2020.1.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/arthur-kamau/Software/clion-2020.1.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/toolman.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/toolman.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/toolman.dir/flags.make

CMakeFiles/toolman.dir/main.cpp.o: CMakeFiles/toolman.dir/flags.make
CMakeFiles/toolman.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/toolman.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/toolman.dir/main.cpp.o -c /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/main.cpp

CMakeFiles/toolman.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/toolman.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/main.cpp > CMakeFiles/toolman.dir/main.cpp.i

CMakeFiles/toolman.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/toolman.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/main.cpp -o CMakeFiles/toolman.dir/main.cpp.s

# Object files for target toolman
toolman_OBJECTS = \
"CMakeFiles/toolman.dir/main.cpp.o"

# External object files for target toolman
toolman_EXTERNAL_OBJECTS =

toolman: CMakeFiles/toolman.dir/main.cpp.o
toolman: CMakeFiles/toolman.dir/build.make
toolman: CMakeFiles/toolman.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable toolman"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/toolman.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/toolman.dir/build: toolman

.PHONY : CMakeFiles/toolman.dir/build

CMakeFiles/toolman.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/toolman.dir/cmake_clean.cmake
.PHONY : CMakeFiles/toolman.dir/clean

CMakeFiles/toolman.dir/depend:
	cd /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/cmake-build-debug /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/cmake-build-debug /home/arthur-kamau/Development/Stratos/compiler/tooling/toolman/cmake-build-debug/CMakeFiles/toolman.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/toolman.dir/depend

