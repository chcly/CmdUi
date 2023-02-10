cmake_minimum_required(VERSION 3.15)
project(CmdUi)

# Use C++-17 by default.
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Set the module path to include scripts
# CMake - Contains project local modules.
#
# CMake/Modules - Contains project independent modules.
#   GitUpdate.cmake invokes python ${CmdUi_SOURCE_DIR}/gitupdate.py
#   to pull all submodules and update to head.
#   If this fails, it will attempt to invoke git directly to pull 
#   submodules. 
list(APPEND CMAKE_MODULE_PATH 
  ${CmdUi_SOURCE_DIR}/CMake 
  ${CmdUi_SOURCE_DIR}/CMake/Modules
)
include(${CmdUi_SOURCE_DIR}/CMake/Configure.cmake)
if (NOT Configure_SUCCEEDED)
   return()
endif()



if (CmdUi_ExternalTarget)
    set(TargetFolders ${CmdUi_TargetFolders})
    set(TargetName    ${CmdUi_TargetName})
    set(TargetGroup   ${CmdUi_TargetGroup})
else()
    add_subdirectory(CMake)
    add_subdirectory(Internal/Utils)

    set(TargetFolders TRUE)
    set(TargetName CmdUi)
    set(TargetGroup )

    set(CmdUi_INCLUDE  ${CmdUi_SOURCE_DIR})
    set(CmdUi_LIBRARY  ${TargetName})
endif()

add_subdirectory(CmdUi)

if (CmdUi_BUILD_TEST)
    if (NOT CmdUi_ExternalTarget)
        add_gtest_source(Extern/ThirdParty)
    endif()

    set(TargetGroup Units)
    add_subdirectory(Test)
endif()
