﻿# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build

# Include any dependencies generated for this target.
include CMakeFiles\blink_test.dir\depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles\blink_test.dir\compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles\blink_test.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\blink_test.dir\flags.make

CMakeFiles\blink_test.dir\blink.c.obj: CMakeFiles\blink_test.dir\flags.make
CMakeFiles\blink_test.dir\blink.c.obj: C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\blink.c
CMakeFiles\blink_test.dir\blink.c.obj: CMakeFiles\blink_test.dir\compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/blink_test.dir/blink.c.obj"
	$(CMAKE_COMMAND) -E cmake_cl_compile_depends --dep-file=CMakeFiles\blink_test.dir\blink.c.obj.d --working-dir=C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build --filter-prefix="Note: including file: " -- "C:\PROGRA~2\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.35.32215\bin\Hostx86\x86\cl.exe" @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /showIncludes /FoCMakeFiles\blink_test.dir\blink.c.obj /FdCMakeFiles\blink_test.dir\ /FS -c C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\blink.c
<<

CMakeFiles\blink_test.dir\blink.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/blink_test.dir/blink.c.i"
	"C:\PROGRA~2\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.35.32215\bin\Hostx86\x86\cl.exe" > CMakeFiles\blink_test.dir\blink.c.i @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\blink.c
<<

CMakeFiles\blink_test.dir\blink.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/blink_test.dir/blink.c.s"
	"C:\PROGRA~2\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.35.32215\bin\Hostx86\x86\cl.exe" @<<
 /nologo $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) /FoNUL /FAs /FaCMakeFiles\blink_test.dir\blink.c.s /c C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\blink.c
<<

# Object files for target blink_test
blink_test_OBJECTS = \
"CMakeFiles\blink_test.dir\blink.c.obj"

# External object files for target blink_test
blink_test_EXTERNAL_OBJECTS =

blink_test.exe: CMakeFiles\blink_test.dir\blink.c.obj
blink_test.exe: CMakeFiles\blink_test.dir\build.make
blink_test.exe: CMakeFiles\blink_test.dir\objects1
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable blink_test.exe"
	"C:\Program Files\CMake\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\blink_test.dir --rc="C:\PROGRA~2\Windows Kits\10\bin\10.0.20348.0\x86\rc.exe" --mt="C:\PROGRA~2\Windows Kits\10\bin\10.0.20348.0\x86\mt.exe" --manifests -- "C:\PROGRA~2\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.35.32215\bin\Hostx86\x86\link.exe" /nologo @CMakeFiles\blink_test.dir\objects1 @<<
 /out:blink_test.exe /implib:blink_test.lib /pdb:C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build\blink_test.pdb /version:0.0 /machine:X86 /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\blink_test.dir\build: blink_test.exe
.PHONY : CMakeFiles\blink_test.dir\build

CMakeFiles\blink_test.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\blink_test.dir\cmake_clean.cmake
.PHONY : CMakeFiles\blink_test.dir\clean

CMakeFiles\blink_test.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build C:\DavidZaha\Programming\Github\EEGLES_Firmware\blink_test\build\CMakeFiles\blink_test.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\blink_test.dir\depend

