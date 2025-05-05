cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(crs LANGUAGES CXX)

include_directories(rear/include)

find_package(Crow CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(FATAL_ERROR "请在Windows下使用MSVC编译器")
    else()
        if(NOT DEFINED WIN32_WINNT)
            # 获取 Windows 系统的版本
            string(REGEX MATCH "([0-9]+)" WIN_VERSION ${CMAKE_SYSTEM_VERSION})
            # 判断 Windows 版本并设置 _WIN32_WINNT
            if(WIN_VERSION VERSION_LESS "10")
                # Windows 7
                add_definitions(-D_WIN32_WINNT=0x0601)
            elseif(WIN_VERSION VERSION_LESS "11")
                # Windows 10
                add_definitions(-D_WIN32_WINNT=0x0A00)
            else()
                # Windows 11
                add_definitions(-D_WIN32_WINNT=0x0A01)
            endif()
        endif()
        add_executable(crs rear/src/main.cpp
                rear/src/Front.cpp
                rear/src/database.cpp
                rear/src/sha256.cpp
        )
        target_link_libraries(crs PRIVATE Crow::Crow asio::asio)
        target_link_libraries(crs PRIVATE unofficial::sqlite3::sqlite3)
        target_link_libraries(crs PRIVATE unofficial::mysql-connector-cpp::connector)
        target_link_libraries(crs PRIVATE OpenSSL::Crypto)
        set_target_properties(crs PROPERTIES OUTPUT_NAME "crs_win_x86-64")
    endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "请在Linux下使用g++编译器")
    else()
        add_executable(crs rear/src/main.cpp
                rear/include/main.h
                rear/src/Front.cpp
                rear/include/Front.h
                rear/src/database.cpp
                rear/include/database.h
                rear/include/cors.h
                rear/include/sha256.h
                rear/src/sha256.cpp
        )
        target_link_libraries(crs PRIVATE Crow::Crow asio::asio)
        target_link_libraries(crs PRIVATE unofficial::sqlite3::sqlite3)
        target_link_libraries(crs PRIVATE unofficial::mysql-connector-cpp::connector)
        set_target_properties(crs PROPERTIES OUTPUT_NAME "crs_linux_x86-64")
    endif()
endif()

file(COPY "front" DESTINATION ${CMAKE_BINARY_DIR}/bin)

if(WIN32)

endif()
