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
        include/KitchenPool.hpp
        src/KitchenPool.cpp
        include/ipc/IpcMessageType.hpp
        include/ipc/IpcMessage.hpp
        src/ipc/IpcMessageType.cpp
        include/ipc/IMessageBus.hpp
        include/ipc/KernelQueueMessageBus.hpp
        include/ipc/ISubscriber.hpp
        src/ipc/CallbackSubscriber.cpp
        include/ipc/CallbackSubscriber.hpp
        include/Kitchen.hpp
        include/Kitchen.hpp
        include/ipc/IpcRoutingKeyHolder.hpp
        src/ipc/IpcRoutingKeyHolder.cpp
        include/Reception.hpp
        include/Order.hpp
        include/KitchenDetails.hpp
        include/ipc/Payloads.hpp
        src/ipc/Payloads.cpp
)


