cmake_minimum_required(VERSION 3.15)
project(hw-02_huffman LANGUAGES CXX)

# Устанавливаем выходную директорию в _build
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR})
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# Настройки компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги компиляции
add_compile_options(-Wall -Wextra -Werror -O0)

# Включаем директории
include_directories(
        include
        include/libs
        src
        test
)

# Получаем список исходников из src/
file(GLOB SRCS "src/*.cpp")
# Исключаем main.cpp для тестов
list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Основной исполняемый файл
add_executable(${PROJECT_NAME}
        src/main.cpp
        ${SRCS}
)

# Тестовый исполняемый файл
add_executable(${PROJECT_NAME}_tests
        test/test.cpp
        ${SRCS}
)

# Устанавливаем выходные директории для исполняемых файлов
set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
)

# Подключаем pthread для тестов
target_link_libraries(${PROJECT_NAME}_tests
        PRIVATE
        pthread
)

# Все остальные артефакты - в стандартную build-директорию
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Дополнительные цели для удобства
add_custom_target(tests
        DEPENDS ${PROJECT_NAME}_tests
        COMMENT "Building tests"
)

add_custom_target(run_tests
        COMMAND ${OUTPUT_DIR}/${PROJECT_NAME}_tests
        DEPENDS ${PROJECT_NAME}_tests
        COMMENT "Running tests"
        WORKING_DIRECTORY ${OUTPUT_DIR}
)

# Цель для очистки
add_custom_target(clean-all
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${OUTPUT_DIR}
        COMMENT "Cleaning all build files and executables"
)
