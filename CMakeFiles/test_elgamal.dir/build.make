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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/CLionProjects/Kunlun

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/CLionProjects/Kunlun

# Include any dependencies generated for this target.
include CMakeFiles/test_elgamal.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_elgamal.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_elgamal.dir/flags.make

CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.o: CMakeFiles/test_elgamal.dir/flags.make
CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.o: test/test_elgamal.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.o -c /home/ubuntu/CLionProjects/Kunlun/test/test_elgamal.cpp

CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/Kunlun/test/test_elgamal.cpp > CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.i

CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/Kunlun/test/test_elgamal.cpp -o CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.s

# Object files for target test_elgamal
test_elgamal_OBJECTS = \
"CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.o"

# External object files for target test_elgamal
test_elgamal_EXTERNAL_OBJECTS =

build/test_elgamal: CMakeFiles/test_elgamal.dir/test/test_elgamal.cpp.o
build/test_elgamal: CMakeFiles/test_elgamal.dir/build.make
build/test_elgamal: /usr/local/lib64/libcrypto.a
build/test_elgamal: /usr/local/lib64/libssl.a
build/test_elgamal: /usr/lib/gcc/x86_64-linux-gnu/9/libgomp.so
build/test_elgamal: /usr/lib/x86_64-linux-gnu/libpthread.so
build/test_elgamal: CMakeFiles/test_elgamal.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable build/test_elgamal"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_elgamal.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_elgamal.dir/build: build/test_elgamal

.PHONY : CMakeFiles/test_elgamal.dir/build

CMakeFiles/test_elgamal.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_elgamal.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_elgamal.dir/clean

CMakeFiles/test_elgamal.dir/depend:
	cd /home/ubuntu/CLionProjects/Kunlun && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun/CMakeFiles/test_elgamal.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_elgamal.dir/depend

