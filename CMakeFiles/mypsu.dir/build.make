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
include CMakeFiles/mypsu.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mypsu.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mypsu.dir/flags.make

CMakeFiles/mypsu.dir/test/mypsuTest.cpp.o: CMakeFiles/mypsu.dir/flags.make
CMakeFiles/mypsu.dir/test/mypsuTest.cpp.o: test/mypsuTest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mypsu.dir/test/mypsuTest.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/mypsu.dir/test/mypsuTest.cpp.o -c /home/ubuntu/CLionProjects/Kunlun/test/mypsuTest.cpp

CMakeFiles/mypsu.dir/test/mypsuTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mypsu.dir/test/mypsuTest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/Kunlun/test/mypsuTest.cpp > CMakeFiles/mypsu.dir/test/mypsuTest.cpp.i

CMakeFiles/mypsu.dir/test/mypsuTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mypsu.dir/test/mypsuTest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/Kunlun/test/mypsuTest.cpp -o CMakeFiles/mypsu.dir/test/mypsuTest.cpp.s

# Object files for target mypsu
mypsu_OBJECTS = \
"CMakeFiles/mypsu.dir/test/mypsuTest.cpp.o"

# External object files for target mypsu
mypsu_EXTERNAL_OBJECTS =

build/mypsu: CMakeFiles/mypsu.dir/test/mypsuTest.cpp.o
build/mypsu: CMakeFiles/mypsu.dir/build.make
build/mypsu: /usr/local/lib64/libcrypto.a
build/mypsu: /usr/local/lib64/libssl.a
build/mypsu: /usr/lib/gcc/x86_64-linux-gnu/9/libgomp.so
build/mypsu: /usr/lib/x86_64-linux-gnu/libpthread.so
build/mypsu: CMakeFiles/mypsu.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable build/mypsu"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mypsu.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mypsu.dir/build: build/mypsu

.PHONY : CMakeFiles/mypsu.dir/build

CMakeFiles/mypsu.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mypsu.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mypsu.dir/clean

CMakeFiles/mypsu.dir/depend:
	cd /home/ubuntu/CLionProjects/Kunlun && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun/CMakeFiles/mypsu.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mypsu.dir/depend

