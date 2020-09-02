cmake_minimum_required(VERSION 3.17)

project(vectors)

# Configure language as C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# -----------------------------------------------------------------------------
# Project CMake Generation settings 

# Suppress generator of ZERO_CHECK project and hide ALL_BUILD project
set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configurations are Debug & Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Set current project as debug project
# https://stackoverflow.com/questions/7304625/how-do-i-change-the-startup-project-of-a-visual-studio-solution-via-cmake/37994396#37994396
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT vectors)

# Generate clang .json compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# -----------------------------------------------------------------------------
# Begin message
if(MSVC)
    message(STATUS "Compiling with MSVC")
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    message(STATUS "Compiling with Clang")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Compiling with GCC")
else()
    message(AUTHOR_WARNING "Unsupported '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()


# -----------------------------------------------------------------------------
# Output directories

# https://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin)
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)



# -----------------------------------------------------------------------------
# File inclusion

# Include only the root project directory
include_directories("./")

# Include for compilation all desired files
# https://stackoverflow.com/questions/31422680/how-to-set-visual-studio-filters-for-nested-sub-directory-using-cmake
# https://stackoverflow.com/questions/3201154/automatically-add-all-files-in-a-folder-to-a-target-using-cmake

file(GLOB_RECURSE source_list
    "src/*.h"
    "src/*.hpp"
    "src/*.c"
    "src/*.cpp"
    "src/*.inl"
    "inc/*.h"
    "inc/*.hpp"
    "inc/*.inl"
    "*.natvis"
)

# Create subfolders and filter desired files
foreach(source IN LISTS source_list)
    # Check if the path is absolute and convert it to relative
    if (IS_ABSOLUTE "${source}")
        file(RELATIVE_PATH source "${CMAKE_CURRENT_SOURCE_DIR}" "${source}")
    endif()

    message(STATUS "${source}")

    # Check if the path is a "build" path
    string(FIND "${source}" "build" isBuildPath)
    if (isBuildPath EQUAL -1)
    else()
        continue()
    endif()


    # Check if the file is a foreign main
    string(FIND "${source}" "main." isMainFile)
    if (isMainFile EQUAL -1)
    else()
        string(COMPARE EQUAL "${source}" "src/main.cpp" isMainCpp)
        string(COMPARE EQUAL "${source}" "src/main.c" isMainC)
        if ((isMainCpp) OR (isMainC))
        else()
            continue()
        endif()
    endif()

    # Generate subfolders from paths 
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")

    # Confirm the file is wanted
    list(APPEND good_sources_list "${source}")
endforeach()


# Create executable
add_executable(vectors ${good_sources_list})




# -----------------------------------------------------------------------------
# Warnings

# https://github.com/lefticus/cpp_starter_project/blob/master/CMakeLists.txt
# https://github.com/lefticus/cpp_starter_project/blob/master/cmake/CompilerWarnings.cmake

# Warning library linking
add_library(project_warnings INTERFACE)

# Create MSVC warnings
set(MSVC_WARNINGS
    /Wall       # All warnings activated
    /wd4191     # remove warnings : 'operator': unsafe conversion from 'type_of_expression' to 'type_required'
    /wd4255     # remove warnings : 'function': no function prototype given: converting '()' to '(void)'
    /wd4263     # remove warnings : 'function': member function does not override any base class virtual member function
    /wd4264     # remove warnings : 'virtual_function': no override available for virtual member function from base 'class'; function is hidden
    /wd4266     # remove warnings : 'function': no override available for virtual member function from base 'type'; function is hidden
    /wd4287     # remove warnings : 'operator': unsigned/negative constant mismatch
    /wd4296     # remove warnings : 'operator': expression is always false
    /wd4339     # remove warnings : 'type' : use of undefined type detected in CLR meta-data - use of this type may lead to a runtime exception
    /wd4342     # remove warnings : behavior change: 'function' called, but a member operator was called in previous versions
    /wd4350     # remove warnings : behavior change: 'member1' called instead of 'member2'
    /wd4365     # remove warnings : 'action': conversion from 'type_1' to 'type_2', signed/unsigned mismatch
    /wd4370     # remove warnings : layout of class has changed from a previous version of the compiler due to better packing
    /wd4371     # remove warnings : 'classname': layout of class may have changed from a previous version of the compiler due to better packing of member 'member'
    /wd4412     # remove warnings : 'function': function signature contains type 'type'; C++ objects are unsafe to pass between pure code and mixed or native
    /wd4426     # remove warnings : optimization flags changed after including header, may be due to #pragma optimize()
    /wd4435     # remove warnings : 'class1' : Object layout under /vd2 will change due to virtual base 'class2'
    /wd4437     # remove warnings : dynamic_cast from virtual base 'class1' to 'class2' could fail in some contexts
    /wd4444     # remove warnings : top level '__unaligned' is not implemented in this context
    /wd4464     # remove warnings : relative include path contains '..'
    /wd4471     # remove warnings : a forward declaration of an unscoped enumeration must have an underlying type (int assumed)
    /wd4472     # remove warnings : 'identifier' is a native enum: add an access specifier (private/public) to declare a managed enum
    /wd4514     # remove warnings : 'function': unreferenced inline function has been removed
    /wd4547     # remove warnings : 'operator': operator before comma has no effect; expected operator with side-effect
    /wd4548     # remove warnings : expression before comma has no effect; expected expression with side-effect
    /wd4557     # remove warnings : '__assume' contains effect 'effect'
    /wd4571     # remove warnings : informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
    /wd4577     # remove warnings : 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
    /wd4587     # remove warnings : 'anonymous_structure': behavior change: constructor is no longer implicitly called
    /wd4588     # remove warnings : 'anonymous_structure': behavior change: destructor is no longer implicitly called
    /wd4598     # remove warnings : '#include "header"': header number header-number in the precompiled header does not match current compilation at that position
    /wd4599     # remove warnings : 'option path': command-line argument number number does not match pre-compiled header
    /wd4605     # remove warnings : '/Dmacro' specified on current command line, but was not specified when precompiled header was built
    /wd4608     # remove warnings : 'union_member' has already been initialized by another union member in the initializer list, 'union_member'
    /wd4626     # remove warnings : 'derived class' : assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
    /wd4628     # remove warnings : digraphs not supported with -Ze. Character sequence 'digraph' not interpreted as alternate token for 'char'
    /wd4640     # remove warnings :  'instance': construction of local static object is not thread-safe
    /wd4647     # remove warnings : behavior change: __is_pod(type) has different value in previous versions
    /wd4668     # remove warnings : 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
    /wd4682     # remove warnings : 'symbol' : no directional parameter attribute specified, defaulting to [in]
    /wd4686     # remove warnings : 'user-defined type': possible change in behavior, change in UDT return calling convention
    /wd4692     # remove warnings : 'function': signature of non-private member contains assembly private native type 'native_type'
    /wd4710     # remove warnings : 'function': function not inlined
    /wd4711     # remove warnings : 'function': selected for automatic inline expansion
    /wd4738     # remove warnings : storing 32-bit float result in memory, possible loss of performance
    /wd4746     # remove warnings : volatile access of 'expression' is subject to /volatile:<iso|ms> setting; consider using __iso_volatile_load/store intrinsic functions
    /wd4749     # remove warnings : conditionally supported: offsetof applied to non-standard-layout type 'type'
    /wd4767     # remove warnings : section name 'symbol' is longer than 8 characters and will be truncated by the linker
    /wd4768     # remove warnings : __declspec attributes before linkage specification are ignored
    /wd4786     # remove warnings : 'symbol' : object name was truncated to 'number' characters in the debug information
    /wd4820     # remove warnings : 'bytes' bytes padding added after construct 'member_name'
    /wd4822     # remove warnings : 'member': local class member function does not have a body
    /wd4826     # remove warnings : Conversion from 'type1' to 'type2' is sign-extended. This may cause unexpected runtime behavior.
    /wd4841     # remove warnings : non-standard extension used: compound member designator used in offsetof
    /wd4842     # remove warnings : the result of 'offsetof' applied to a type using multiple inheritance is not guaranteed to be consistent between compiler releases
    /wd4917     # remove warnings : 'declarator': a GUID can only be associated with a class, interface, or namespace
    /wd4928     # remove warnings : illegal copy-initialization; more than one user-defined conversion has been implicitly applied
    /wd4931     # remove warnings : we are assuming the type library was built for number-bit pointers
    /wd4946     # remove warnings : reinterpret_cast used between related classes: 'class1' and 'class2'
    /wd4962     # remove warnings : 'function': profile-guided optimizations disabled because optimizations caused profile data to become inconsistent
    /wd4986     # remove warnings : 'symbol': exception specification does not match previous declaration
    /wd5022     # remove warnings : 'type': multiple move constructors specified
    /wd5023     # remove warnings : 'type': multiple move assignment operators specified
    /wd5029     # remove warnings : nonstandard extension used: alignment attributes in C++ apply to variables, data members and tag types only
    /wd5034     # remove warnings : use of intrinsic 'intrinsic' causes function function-name to be compiled as guest code
    /wd5035     # remove warnings : use of feature 'feature' causes function function-name to be compiled as guest code
    /wd5038     # remove warnings : data member 'member1' will be initialized after data member 'member2'
    /wd5039     # remove warnings : 'function': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
    /wd5042     # remove warnings : 'function': function declarations at block scope cannot be specified 'inline' in standard C++; remove 'inline' specifier
    /wd5045     # remove warnings : Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
)

# Add Control Flow Guard on MSVC
set(MSVC_WARNINGS ${MSVC_WARNINGS} /guard:cf)


# Create Clang Warnings
set(CLANG_WARNINGS
      -Wall
      -Wextra
      -Wshadow              # warn the user if a variable declaration shadows one from a parent context
      -Wnon-virtual-dtor    # warn the user if a class with virtual functions has a non-virtual destructor.
      -Wold-style-cast      # warn for c-style casts
      -Wcast-align          # warn for potential performance problem casts
      -Wunused              # warn on anything being unused
      -Woverloaded-virtual  # warn if you overload (not override) a virtual function
      -Wconversion          # warn on type conversions that may lose data
      -Wsign-conversion     # warn on sign conversions
      -Wnull-dereference    # warn if a null dereference is detected
      -Wdouble-promotion    # warn if float is implicit promoted to double
      -Wformat=2            # warn on security issues around functions that format output (ie printf)
)

set(GCC_WARNINGS
      ${CLANG_WARNINGS}         # Add all clang warnings
      -Wmisleading-indentation  # warn if indentation implies blocks where blocks do not exist
      -Wduplicated-cond         # warn if if / else chain has duplicated conditions
      -Wduplicated-branches     # warn if if / else branches have duplicated code
      -Wlogical-op              # warn about logical operations being used where bitwise were probably wanted
      -Wuseless-cast            # warn if you perform a cast to the same type
  )

# Set warnings as errors
set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
set(GCC_WARNINGS ${GCC_WARNINGS} -Werror)
set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)

# Select a warning set
if(MSVC)
	set(PROJECT_WARNINGS ${MSVC_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_WARNINGS ${CLANG_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_WARNINGS ${GCC_WARNINGS})
else()
    message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
endif()

# Apply selected warnings
target_compile_options(vectors PRIVATE ${PROJECT_WARNINGS})

# -----------------------------------------------------------------------------
# Clang sanitizers

# These can be selected (one at the time), by invoking CMake with Clang as 
# compiler, and passing "-DPEL_CLANG_USE_XXXXX_SANITIZER=True" as parameter.

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    if(PEL_CLANG_USE_ADDRESS_SANITIZER)
       message(STATUS "Enabling Clang's Address Sanitizer") 
       target_compile_options(vectors INTERFACE -fsanitize=address)
       target_link_libraries(vectors INTERFACE -fsanitize=address)
    elseif(PEL_CLANG_USE_THREAD_SANITIZER)
       message(STATUS "Enabling Clang's Thread Sanitizer")
       target_compile_options(vectors INTERFACE -fsanitize=thread)
       target_link_libraries(vectors INTERFACE -fsanitize=thread)
    elseif(PEL_CLANG__USE_LEAK_SANITIZER)
       message(STATUS "Enabling Clang's Memory Leak Sanitizer")
       target_compile_options(vectors INTERFACE -fsanitize=leak)
       target_link_libraries(vectors INTERFACE -fsanitize=leak)
    elseif(PEL_CLANG_USE_UNDEFINED_BEHAVIOR_SANITIZER)
       message(STATUS "Enabling Clang's Undefined Behavior Sanitizer")
       target_compile_options(vectors INTERFACE -fsanitize=undefined)
       target_link_libraries(vectors INTERFACE -fsanitize=undefined)
    endif()
endif()


# -----------------------------------------------------------------------------
# Misc options

# Select the unicode character set
add_definitions(-D_UNICODE -DUNICODE)

# Enable the console
add_definitions(-D_CONSOLE)
