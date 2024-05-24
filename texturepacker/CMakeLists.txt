cmake_minimum_required(VERSION 3.5)

project (TexturePacker)
set(CMAKE_CXX_STANDARD 17)

if(APPLE AND EXISTS /usr/local/opt/qt5)
	# Homebrew installs Qt5 (up to at least 5.9.1) in
	# /usr/local/qt5, ensure it can be found by CMake since
	# it is not in the default /usr/local prefix.
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()

# 启用自动UI编译（AUTOUIC）
set(CMAKE_AUTOUIC ON)
# 启用自动元对象编译（AUTOMOC）
set(CMAKE_AUTOMOC ON)
# 启用自动资源编译（AUTORCC）
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
if(Qt5_FOUND)
	add_executable(${PROJECT_NAME} main_qt.cpp)
	target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Widgets)
else()
	add_executable(${PROJECT_NAME} main.cpp)
endif()


