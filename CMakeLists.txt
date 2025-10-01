cmake_minimum_required(VERSION 3.20)

# Change this line to change the name of the ROM
set(PROJECT_NAME step_by_step)

set(CMAKE_EXECUTABLE_SUFFIX_CXX ".nes")

set(LAUNCH_NES_FILE_AFTER_BUILD Off CACHE BOOL "Should I open the NES file after compilation is complete?")
set(LAUNCH_NES_FILE_EMULATOR_PATH "" CACHE PATH "If provided, use this emulator to load the ROM, otherwise default to whatever the OS chooses")

# Set this to Off to disable downloading the SDK for you
option(LLVM_MOS_BOOTSTRAP_SDK "Should I download the llvm-mos SDK?" On)
include(bootstrap-sdk)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(LLVM_MOS_PLATFORM nes-nrom)
find_package(llvm-mos-sdk REQUIRED)

project(${PROJECT_NAME} C CXX ASM)

# Include all source files in the src directory as build objects
file(GLOB_RECURSE SRCS
    CONFIGURE_DEPENDS
    "src/*"
)
add_executable(${CMAKE_PROJECT_NAME} ${SRCS})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

include(add-ca65-folder)
add_ca65_source(TARGET ${CMAKE_PROJECT_NAME} SRC ${CMAKE_SOURCE_DIR}/ca65)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -g -gdwarf-4           # We want debug info generated for all builds
    -Wall -Wextra -Werror  # Compiler's got your back
    -Wno-error=unused-command-line-argument # But compiling asm files *will* add a lot of unused cli args so ignore these
    -Wno-c23-extensions # just let me use #embed pls and thx u
    -Wno-c99-extensions # and array designators!
    -Wno-vla-cxx-extension
    -Wno-mismatched-tags
    -fno-unroll-loops
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -g -gdwarf-4
    -Wall -Wextra -Werror # Same goes for the linker
    -Wno-error=unused-command-line-argument
    # !! NOTICE !! If you reserve any ZEROPAGE variables in ca65, then you need to calculate how much ZP its using and
    # update this value with the total number of bytes of ZP that your ca65 code uses.
    -mreserve-zp=42

    # make functions non-recursive by default
    -fnonreentrant
    -fno-unroll-loops
    # -Map generates a file with information about how the final binary's memory
    # is laid out. This will eventually be used to show the remaining space in
    # CHR-ROM, PRG-ROM, PRG-RAM, System RAM, and ZEROPAGE
    
    # --lto-whole-program-visibility helps ensure clang does as many
    # optimizations as possible
    -Wl,-Map,${CMAKE_PROJECT_NAME}.map,--lto-whole-program-visibility
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    # Second-party, included in llvm-mos
    neslib
    nesdoug
)

if (LAUNCH_NES_FILE_AFTER_BUILD)
    if (LAUNCH_NES_FILE_EMULATOR_PATH)
        add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}
            POST_BUILD
            COMMAND ${LAUNCH_NES_FILE_EMULATOR_PATH} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            COMMENT "Running ${CMAKE_PROJECT_NAME} after successful build"
        )
    else()
        add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}
            POST_BUILD
            COMMAND $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            COMMENT "Running ${CMAKE_PROJECT_NAME} after successful build"
        )
    endif()
endif()
