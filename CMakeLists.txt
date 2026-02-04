cmake_minimum_required(VERSION 3.12...3.31)
project(Allocation_userver CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(
    userver
    COMPONENTS core postgresql
    QUIET
)
if(NOT userver_FOUND)
    # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
    download_userver(TRY_DIR third_party/userver)
endif()

userver_setup_environment()

# Common sources
include_directories(src)

add_library(
    ${PROJECT_NAME}_objs OBJECT
    src/Domain/Product/Batch.cpp
    src/Domain/Product/Product.cpp
    src/Domain/Ports/IMessage.hpp
    src/Domain/Ports/IRepository.hpp
    src/Adapters/Repository/TrackingRepository.hpp
    src/Adapters/Repository/TrackingRepository.cpp
    src/Adapters/Repository/SqlRepository.hpp
    src/Adapters/Repository/SqlRepository.cpp
    src/Adapters/Repository/DTO.hpp
    #src/ServiceLayer/MessageBus/MessageBus.cpp
    #src/Adapters/MessageBus/MessageBusComponent.cpp
)

userver_add_sql_library(
    ${PROJECT_NAME}_sql
    NAMESPACE
    Allocation
    SOURCE_DIR
    ${CMAKE_SOURCE_DIR}/postgresql/queries
    OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    SQL_FILES
    *.sql
)

target_link_libraries(
    ${PROJECT_NAME}_objs
    PUBLIC
    ${PROJECT_NAME}_sql
    userver::core #
)
target_precompile_headers(${PROJECT_NAME}_objs PUBLIC ${CMAKE_SOURCE_DIR}/src/Precompile.hpp)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# Unit Tests
add_executable(
    ${PROJECT_NAME}_unittest
    src/Tests/Unit/Batches.cpp
    src/Tests/Unit/Products.cpp
    #src/Tests/Unit/Handlers.cpp
    #src/Tests/Utilities/Common.cpp
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)

# Benchmarks
# add_executable(${PROJECT_NAME}_benchmark src/greeting_benchmark.cpp)
# target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
# add_google_benchmark_tests(${PROJECT_NAME}_benchmark)

# Functional testing
userver_testsuite_add_simple()

# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
    message(STATUS "Set install prefix: $ENV{PREFIX}")
    file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
    set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
)
install(
    FILES ${CONFIGS_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)