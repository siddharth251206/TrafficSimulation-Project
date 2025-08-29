# --- vcpkg Root Finder Script ---

function(find_vcpkg_root)
    set(VCPKG_SEARCH_PATHS)

    if(DEFINED VCPKG_ROOT)
        list(APPEND VCPKG_SEARCH_PATHS "${VCPKG_ROOT}")
    endif()

    if(DEFINED ENV{VCPKG_ROOT})
        list(APPEND VCPKG_SEARCH_PATHS "$ENV{VCPKG_ROOT}")
    endif()

    if(WIN32)
        list(APPEND VCPKG_SEARCH_PATHS
            "C:/vcpkg"
            "C:/tools/vcpkg"
            "C:/dev/vcpkg"
            "$ENV{USERPROFILE}/vcpkg"
            "$ENV{LOCALAPPDATA}/vcpkg"
        )
    else()
        list(APPEND VCPKG_SEARCH_PATHS
            "/usr/local/vcpkg"
            "/opt/vcpkg"
            "$ENV{HOME}/vcpkg"
            "${CMAKE_SOURCE_DIR}/../vcpkg"
            "${CMAKE_SOURCE_DIR}/third_party/vcpkg"
        )
    endif()

    # Try to find vcpkg executable in PATH
    find_program(VCPKG_EXECUTABLE_TEMP
        NAMES vcpkg vcpkg.exe
        PATHS ENV PATH
    )
    if(VCPKG_EXECUTABLE_TEMP)
        get_filename_component(VCPKG_EXECUTABLE_DIR "${VCPKG_EXECUTABLE_TEMP}" DIRECTORY)
        list(APPEND VCPKG_SEARCH_PATHS "${VCPKG_EXECUTABLE_DIR}")
    endif()

    # Search for valid vcpkg installation
    foreach(path IN LISTS VCPKG_SEARCH_PATHS)
        if(EXISTS "${path}/scripts/buildsystems/vcpkg.cmake")
            set(VCPKG_ROOT_FOUND "${path}" PARENT_SCOPE)
            set(VCPKG_TOOLCHAIN_PATH "${path}/scripts/buildsystems/vcpkg.cmake" PARENT_SCOPE)
            message(STATUS "vcpkg found at: ${path}")
            return()
        endif()
    endforeach()

    # If nothing found, set empty values
    set(VCPKG_ROOT_FOUND "" PARENT_SCOPE)
    set(VCPKG_TOOLCHAIN_PATH "" PARENT_SCOPE)
    message(WARNING
        "vcpkg not found! To install vcpkg:\n"
        "  Windows: git clone https://github.com/Microsoft/vcpkg.git C:\\vcpkg && C:\\vcpkg\\bootstrap-vcpkg.bat\n"
        "  Linux/Mac: git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg && ~/vcpkg/bootstrap-vcpkg.sh\n"
        "Then set VCPKG_ROOT environment variable or use CMakePresets.json"
    )
endfunction()

# Function to validate vcpkg installation
function(validate_vcpkg_installation vcpkg_root)
    if(NOT EXISTS "${vcpkg_root}")
        message(FATAL_ERROR "vcpkg root directory does not exist: ${vcpkg_root}")
    endif()

    if(NOT EXISTS "${vcpkg_root}/scripts/buildsystems/vcpkg.cmake")
        message(FATAL_ERROR "vcpkg toolchain not found at: ${vcpkg_root}/scripts/buildsystems/vcpkg.cmake")
    endif()

    # Check for vcpkg executable
    set(vcpkg_exe "${vcpkg_root}/vcpkg")
    if(WIN32)
        set(vcpkg_exe "${vcpkg_root}/vcpkg.exe")
    endif()

    if(NOT EXISTS "${vcpkg_exe}")
        message(FATAL_ERROR "vcpkg executable not found at: ${vcpkg_exe}")
    endif()

    message(STATUS "vcpkg installation validated at: ${vcpkg_root}")
endfunction()

# Main detection logic (only runs if called directly)
if(CMAKE_SCRIPT_MODE_FILE)
    find_vcpkg_root()
    if(VCPKG_ROOT_FOUND)
        validate_vcpkg_installation("${VCPKG_ROOT_FOUND}")
        message(STATUS "VCPKG_ROOT=${VCPKG_ROOT_FOUND}")
        message(STATUS "VCPKG_TOOLCHAIN_FILE=${VCPKG_TOOLCHAIN_PATH}")
    else()
        message(FATAL_ERROR "vcpkg not found")
    endif()
endif()
