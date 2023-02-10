include(CopyTargets)

macro(make_exe TargetName MAIN)
    set(TargetGroup Examples)

    include_directories(${Utils_INCLUDE} ${CmdUi_INCLUDE})
    add_executable(${TargetName} ${MAIN})
    target_link_libraries(${TargetName} ${Utils_LIBRARY} ${CmdUi_LIBRARY})

    set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetGroup}")

    copy_target(${TargetName} ${CMAKE_BINARY_DIR})
endmacro()



make_exe(HelloWorld Samp001.cpp)

