# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /cygdrive/c/Users/31409/.CLion2019.3/system/cygwin_cmake/bin/cmake.exe

# The command to remove a file.
RM = /cygdrive/c/Users/31409/.CLion2019.3/system/cygwin_cmake/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/c/Users/31409/CLionProjects/program3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/program3.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/program3.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/program3.dir/flags.make

CMakeFiles/program3.dir/driver.cpp.o: CMakeFiles/program3.dir/flags.make
CMakeFiles/program3.dir/driver.cpp.o: ../driver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/program3.dir/driver.cpp.o"
	/cygdrive/c/cygwin64/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program3.dir/driver.cpp.o -c /cygdrive/c/Users/31409/CLionProjects/program3/driver.cpp

CMakeFiles/program3.dir/driver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program3.dir/driver.cpp.i"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/31409/CLionProjects/program3/driver.cpp > CMakeFiles/program3.dir/driver.cpp.i

CMakeFiles/program3.dir/driver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program3.dir/driver.cpp.s"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/31409/CLionProjects/program3/driver.cpp -o CMakeFiles/program3.dir/driver.cpp.s

CMakeFiles/program3.dir/test_priority_queue.cpp.o: CMakeFiles/program3.dir/flags.make
CMakeFiles/program3.dir/test_priority_queue.cpp.o: ../test_priority_queue.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/program3.dir/test_priority_queue.cpp.o"
	/cygdrive/c/cygwin64/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program3.dir/test_priority_queue.cpp.o -c /cygdrive/c/Users/31409/CLionProjects/program3/test_priority_queue.cpp

CMakeFiles/program3.dir/test_priority_queue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program3.dir/test_priority_queue.cpp.i"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/31409/CLionProjects/program3/test_priority_queue.cpp > CMakeFiles/program3.dir/test_priority_queue.cpp.i

CMakeFiles/program3.dir/test_priority_queue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program3.dir/test_priority_queue.cpp.s"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/31409/CLionProjects/program3/test_priority_queue.cpp -o CMakeFiles/program3.dir/test_priority_queue.cpp.s

CMakeFiles/program3.dir/test_map.cpp.o: CMakeFiles/program3.dir/flags.make
CMakeFiles/program3.dir/test_map.cpp.o: ../test_map.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/program3.dir/test_map.cpp.o"
	/cygdrive/c/cygwin64/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program3.dir/test_map.cpp.o -c /cygdrive/c/Users/31409/CLionProjects/program3/test_map.cpp

CMakeFiles/program3.dir/test_map.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program3.dir/test_map.cpp.i"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/31409/CLionProjects/program3/test_map.cpp > CMakeFiles/program3.dir/test_map.cpp.i

CMakeFiles/program3.dir/test_map.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program3.dir/test_map.cpp.s"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/31409/CLionProjects/program3/test_map.cpp -o CMakeFiles/program3.dir/test_map.cpp.s

CMakeFiles/program3.dir/wordgenerator.cpp.o: CMakeFiles/program3.dir/flags.make
CMakeFiles/program3.dir/wordgenerator.cpp.o: ../wordgenerator.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/program3.dir/wordgenerator.cpp.o"
	/cygdrive/c/cygwin64/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program3.dir/wordgenerator.cpp.o -c /cygdrive/c/Users/31409/CLionProjects/program3/wordgenerator.cpp

CMakeFiles/program3.dir/wordgenerator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program3.dir/wordgenerator.cpp.i"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/31409/CLionProjects/program3/wordgenerator.cpp > CMakeFiles/program3.dir/wordgenerator.cpp.i

CMakeFiles/program3.dir/wordgenerator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program3.dir/wordgenerator.cpp.s"
	/cygdrive/c/cygwin64/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/31409/CLionProjects/program3/wordgenerator.cpp -o CMakeFiles/program3.dir/wordgenerator.cpp.s

# Object files for target program3
program3_OBJECTS = \
"CMakeFiles/program3.dir/driver.cpp.o" \
"CMakeFiles/program3.dir/test_priority_queue.cpp.o" \
"CMakeFiles/program3.dir/test_map.cpp.o" \
"CMakeFiles/program3.dir/wordgenerator.cpp.o"

# External object files for target program3
program3_EXTERNAL_OBJECTS =

program3.exe: CMakeFiles/program3.dir/driver.cpp.o
program3.exe: CMakeFiles/program3.dir/test_priority_queue.cpp.o
program3.exe: CMakeFiles/program3.dir/test_map.cpp.o
program3.exe: CMakeFiles/program3.dir/wordgenerator.cpp.o
program3.exe: CMakeFiles/program3.dir/build.make
program3.exe: CMakeFiles/program3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable program3.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/program3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/program3.dir/build: program3.exe

.PHONY : CMakeFiles/program3.dir/build

CMakeFiles/program3.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/program3.dir/cmake_clean.cmake
.PHONY : CMakeFiles/program3.dir/clean

CMakeFiles/program3.dir/depend:
	cd /cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/c/Users/31409/CLionProjects/program3 /cygdrive/c/Users/31409/CLionProjects/program3 /cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug /cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug /cygdrive/c/Users/31409/CLionProjects/program3/cmake-build-debug/CMakeFiles/program3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/program3.dir/depend

