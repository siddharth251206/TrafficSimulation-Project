# --- C++ Standard ---

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 23)
endif()


# --- Compiler Extensions ---

set(CMAKE_CXX_EXTENSIONS OFF)


# --- Tooling ---

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Provide compile_commands.json in the project root for clangd/VSCode
# Only applicable for single-config generators that actually emit compile_commands (Ninja/Unix)
if(CMAKE_EXPORT_COMPILE_COMMANDS AND (CMAKE_GENERATOR MATCHES "Ninja|Unix Makefiles"))
  if(WIN32)
    # On Windows, creating symlinks commonly requires admin privileges (especially under OneDrive).
    # Use a regular file copy instead to avoid permission failures.
    add_custom_target(
      symlink_compile_commands ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              ${CMAKE_BINARY_DIR}/compile_commands.json
              ${CMAKE_SOURCE_DIR}/compile_commands.json
      DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
      COMMENT "Copying compile_commands.json to project root (Windows)"
    )
  else()
    add_custom_target(
      symlink_compile_commands ALL
      COMMAND ${CMAKE_COMMAND} -E create_symlink
              ${CMAKE_BINARY_DIR}/compile_commands.json
              ${CMAKE_SOURCE_DIR}/compile_commands.json
      DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
      COMMENT "Creating symlink to compile_commands.json in project root"
    )
  endif()
endif()


# --- Build Configuration ---

# Disallow in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Default to Release build if unset
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()
