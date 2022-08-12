cmake_minimum_required(VERSION 3.16)
project(async_server)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)
find_package(Boost COMPONENTS system thread REQUIRED) # asio requires system

set(ASYNC_SERVER_FILES
  # ${PROJECT_SOURCE_DIR}/file_name.cpp
  ${PROJECT_SOURCE_DIR}/src/server.cpp
  ${PROJECT_SOURCE_DIR}/src/client.cpp
  ${PROJECT_SOURCE_DIR}/src/session.cpp
)

include_directories(include)

function(add_exec arg1)
  add_executable(${arg1}
    ${arg1}.cpp
    "${ASYNC_SERVER_FILES}"
  )

  target_link_libraries(${arg1}
    Threads::Threads
    "${Boost_LIBRARIES}"
  )  

  target_include_directories(${arg1} PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
  )

  target_compile_features(${arg1} PRIVATE cxx_std_17)  
endfunction()

add_exec(launch_server)
add_exec(launch_client)

