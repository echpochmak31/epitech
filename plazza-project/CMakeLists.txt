cmake_minimum_required(VERSION 3.28)
project(plazza_project)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(plazza_project src/main.cpp
        src/IngredientStock.cpp
        src/Kitchen.cpp
        src/Reception.cpp
        include/KitchenPool.hpp
        src/KitchenPool.cpp
        include/ipc/IpcMessageType.hpp
        include/ipc/IpcMessage.hpp
        include/ipc/IMessageBus.hpp
        include/ipc/KernelQueueMessageBus.hpp
        include/Kitchen.hpp
        include/Kitchen.hpp
        include/ipc/IpcRoutingKeyHolder.hpp
        src/ipc/IpcRoutingKeyHolder.cpp
        include/Reception.hpp
        include/KitchenDetails.hpp
        include/ipc/Payloads.hpp
        src/ipc/Payloads.cpp
        include/RecipeBook.hpp
        include/ThreadSafeQueue.hpp
        include/Constants.hpp
        include/Logger.hpp
        src/Logger.cpp
        include/Utils.hpp
        src/Utils.cpp
        src/RecipeBook.cpp
        include/Order.hpp
        include/KitchenParams.hpp
)


