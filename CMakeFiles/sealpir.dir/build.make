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
include CMakeFiles/sealpir.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/sealpir.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sealpir.dir/flags.make

CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.o: CMakeFiles/sealpir.dir/flags.make
CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.o: mpc/pir/pir.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.o -c /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir.cpp

CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir.cpp > CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.i

CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir.cpp -o CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.s

CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.o: CMakeFiles/sealpir.dir/flags.make
CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.o: mpc/pir/pir_client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.o -c /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir_client.cpp

CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir_client.cpp > CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.i

CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir_client.cpp -o CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.s

CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.o: CMakeFiles/sealpir.dir/flags.make
CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.o: mpc/pir/pir_server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.o -c /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir_server.cpp

CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir_server.cpp > CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.i

CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/CLionProjects/Kunlun/mpc/pir/pir_server.cpp -o CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.s

# Object files for target sealpir
sealpir_OBJECTS = \
"CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.o" \
"CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.o" \
"CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.o"

# External object files for target sealpir
sealpir_EXTERNAL_OBJECTS =

libsealpir.a: CMakeFiles/sealpir.dir/mpc/pir/pir.cpp.o
libsealpir.a: CMakeFiles/sealpir.dir/mpc/pir/pir_client.cpp.o
libsealpir.a: CMakeFiles/sealpir.dir/mpc/pir/pir_server.cpp.o
libsealpir.a: CMakeFiles/sealpir.dir/build.make
libsealpir.a: CMakeFiles/sealpir.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/CLionProjects/Kunlun/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libsealpir.a"
	$(CMAKE_COMMAND) -P CMakeFiles/sealpir.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sealpir.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sealpir.dir/build: libsealpir.a

.PHONY : CMakeFiles/sealpir.dir/build

CMakeFiles/sealpir.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sealpir.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sealpir.dir/clean

CMakeFiles/sealpir.dir/depend:
	cd /home/ubuntu/CLionProjects/Kunlun && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun /home/ubuntu/CLionProjects/Kunlun/CMakeFiles/sealpir.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sealpir.dir/depend
