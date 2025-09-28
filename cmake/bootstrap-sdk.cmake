# If the LLVM_MOS_BOOTSTRAP_SDK variable is enabled, then attempt to
# download it and use it.

include(find-mos-compiler)
find_mos_compiler(LLVM_MOS_C_COMPILER mos-clang)
find_mos_compiler(LLVM_MOS_CXX_COMPILER mos-clang++)
find_mos_compiler(LLVM_MOS_ASM_COMPILER mos-clang)
if(LLVM_MOS_BOOTSTRAP_SDK
  AND NOT LLVM_MOS_C_COMPILER
  AND NOT LLVM_MOS_CXX_COMPILER
  AND NOT LLVM_MOS_ASM_COMPILER)

  if(NOT LLVM_MOS_PRECOMPILED_SDK)
    set(LLVM_MOS_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME})
    if(LLVM_MOS_SYSTEM_NAME STREQUAL "Darwin")
      set(LLVM_MOS_SYSTEM_NAME "macos")
    endif()
    string(TOLOWER ${LLVM_MOS_SYSTEM_NAME} _cmake_host_system_name_lowercase)
    set(LLVM_MOS_PRECOMPILED_SDK llvm-mos-${_cmake_host_system_name_lowercase}
            CACHE STRING "The name of the llvm-mos release to download for precompiled tools")
  endif()
  if (WIN32)
    set(extension 7z)
  else()
    set(extension tar.xz)
  endif()
  include(FetchContent)
  FetchContent_Declare(
          llvm-mos-sdk
          URL https://github.com/llvm-mos/llvm-mos-sdk/releases/latest/download/${LLVM_MOS_PRECOMPILED_SDK}.${extension}
          DOWNLOAD_EXTRACT_TIMESTAMP Off
  )
  message("-- Downloading LLVM-MOS SDK, please wait...")
  FetchContent_MakeAvailable(llvm-mos-sdk)
  
  if (llvm-mos-sdk_POPULATED)
    message("-- LLVM-MOS SDK downloaded")
  else()
    message(SEND_ERROR "Could not download LLVM-MOS SDK; please try again or build it yourself")
  endif()

  list(APPEND CMAKE_PREFIX_PATH "${llvm-mos-sdk_SOURCE_DIR}")
endif()
