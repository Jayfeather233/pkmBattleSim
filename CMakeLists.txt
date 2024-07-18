cmake_minimum_required(VERSION 3.11)

project(pbs)

add_definitions(-DUSE_OPENSSL)

# find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)
# include_directories(${OPENSSL_INCLUDE_DIR})

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=1 )
find_package(ImageMagick COMPONENTS Magick++ MagickWand MagickCore REQUIRED)
include_directories(${ImageMagick_INCLUDE_DIRS})
include_directories(${fmt_INCLUDE_DIRS})
include_directories("./src/shinxbot_api")
include_directories("./src/basic_models")
include_directories("./src/thread")
include_directories("./src/ui")
include_directories("./src/utils")

aux_source_directory(./src/basic_models BM_SOURCES)
aux_source_directory(./src/thread TH_SOURCES)
aux_source_directory(./src/ui UI_SOURCES)
aux_source_directory(./src/utils UTILS_SOURCES)
aux_source_directory(./src MAIN_SOURCES)

# Set the mode you want (e.g., Mode1 or Mode2)
set(MODE "Mode" CACHE STRING "Build mode: main or test")
message(MODE="${MODE}")

# Add all source files in the ./src directory to MAIN_SOURCES
aux_source_directory(./src MAIN_SOURCES)

# Exclude A.cpp in Mode1
if(MODE STREQUAL "main")
    list(REMOVE_ITEM MAIN_SOURCES ./src/test.cpp)
endif()

# Exclude B.cpp in Mode2
if(MODE STREQUAL "test")
    list(REMOVE_ITEM MAIN_SOURCES ./src/main.cpp)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_link_options("-fuse-ld=lld")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstandalone-debug")
endif()

# add_executable(${PROJECT_NAME}  ${BM_SOURCES} ${TH_SOURCES} ${UI_SOURCES} ${UTILS_SOURCES} ${MAIN_SOURCES})

add_library(${PROJECT_NAME} SHARED ${BM_SOURCES} ${TH_SOURCES} ${UI_SOURCES} ${UTILS_SOURCES} ${MAIN_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/../src/shinxbot_api/libutils.so)

target_link_libraries(${PROJECT_NAME} PUBLIC ${ImageMagick_LIBRARIES})

find_package(jsoncpp REQUIRED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${JSONCPP_LIBRARIES})
# target_link_libraries(${PROJECT_NAME} PUBLIC zip)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC -lcurl)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC -ljsoncpp)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC -pthread)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC -lm)
# target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
