cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(system_for_robots)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough"
    )

set(LOG_SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/log_system/logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/log_system/logging.hpp
    )
set(ROBOT ${CMAKE_CURRENT_SOURCE_DIR}/robot/robot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/robot/robot.hpp
    )
set(FIELD ${CMAKE_CURRENT_SOURCE_DIR}/field/field.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/field/field.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/field/cell.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/field/cell.hpp
    )
set(INPUT_SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/input_system/parser_json.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/input_system/parser_json.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_system/json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_system/json.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_system/collector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/input_system/collector.hpp
    )
set(MAIN_SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/system/system.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/system/system.hpp    
    ${CMAKE_CURRENT_SOURCE_DIR}/system/navigator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/system/navigator.hpp
    )
set(TESTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/duration.hpp
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/field
    ${CMAKE_CURRENT_SOURCE_DIR}/input_system
    ${CMAKE_CURRENT_SOURCE_DIR}/log_system
    ${CMAKE_CURRENT_SOURCE_DIR}/robot
    ${CMAKE_CURRENT_SOURCE_DIR}/system 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    ${CMAKE_CURRENT_SOURCE_DIR}/
    )

add_executable(
    system_for_robots
    ${FIELD}
    ${INPUT_SYSTEM}
    ${LOG_SYSTEM}
    ${ROBOT}
    ${MAIN_SYSTEM}
    ${TESTS}
    common.hpp
    main.cpp
    )

target_link_libraries(system_for_robots)

# mkdir build       # Создаем папку для сборки
# cd build          # Переходим в нее
# cmake ..          # Генерируем Makefile
# make              # Компилируем проект
# ./my_program      # Запускаем программу
