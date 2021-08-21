cmake_minimum_required(VERSION 3.16)
project(cryptobot)

set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/extlibs/SFML/include")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/binance-cxx-api/include")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/imgui")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/imgui/misc/cpp")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/imgui-sfml")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/implot")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/ChaiScript/include")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/ChaiScript")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/json/single_include")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/restclient-cpp/include")

add_definitions(-DCHAISCRIPT_NO_THREADS)
add_definitions(-DCHAISCRIPT_NO_THREADS_WARNING)

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS})
file(GLOB_RECURSE CRYPTOBOT_SRC "src/*.cpp" "include/*.hpp")

add_executable(cryptobot ${CRYPTOBOT_SRC} extlibs/implot/implot.cpp extlibs/implot/implot_items.cpp extlibs/imgui/imgui.cpp extlibs/imgui/misc/cpp/imgui_stdlib.cpp extlibs/imgui/imgui_widgets.cpp extlibs/imgui/imgui_draw.cpp extlibs/imgui/imgui_tables.cpp extlibs/imgui-sfml/imgui-SFML.cpp)

link_directories("/usr/local/lib")

target_link_libraries(cryptobot entityx)
target_link_libraries(cryptobot jsoncpp)
target_link_libraries(cryptobot sfml-graphics)
target_link_libraries(cryptobot sfml-window)
target_link_libraries(cryptobot sfml-network)
target_link_libraries(cryptobot sfml-system)
target_link_libraries(cryptobot ${OPENGL_LIBRARIES})
target_link_libraries(cryptobot dl)
target_link_libraries(cryptobot binance-cxx-api)
target_link_libraries(cryptobot restclient-cpp)
target_link_libraries(cryptobot pthread)