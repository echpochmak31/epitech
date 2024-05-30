cmake_minimum_required(VERSION 3.28)
project(plazza_project)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(plazza_project src/main.cpp
        src/Cook.cpp
        src/IngredientStock.cpp
        src/IPC.cpp
        src/Kitchen.cpp
        src/Reception.cpp
        src/Pizza.cpp
)


