cmake_minimum_required(VERSION 3.28)
project(DeskPal LANGUAGES CXX)

# C++17 사용 (SFML 3의 최소 요구 사항)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 3.1.0을 GitHub에서 자동으로 받아와 함께 빌드
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# 실행 파일
add_executable(DeskPal 
        src/main.cpp
        src/App.cpp
        src/Pet.cpp
        src/Animator.cpp
        src/Win32Window.cpp
)
target_link_libraries(DeskPal PRIVATE SFML::Graphics)

# 빌드할 때마다 assets 폴더를 실행 파일 옆으로 복사
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:DeskPal>/assets
)
add_dependencies(DeskPal copy_assets)

# 소스 파일을 UTF-8로 읽기 (한글 주석/문자열 깨짐 방지)
if(MSVC)
    target_compile_options(DeskPal PRIVATE /utf-8)
endif()