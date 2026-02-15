cmake_minimum_required(VERSION 3.12...3.31)
project(Allocation_userver CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(
    userver
    COMPONENTS core postgresql redis chaotic
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
    src/Domain/Commands.cpp
    src/Domain/Commands.hpp
    src/Domain/Events.hpp
    src/Domain/Events.cpp
    src/Domain/Product/OrderLine.hpp
    src/Domain/Product/Batch.hpp
    src/Domain/Product/Batch.cpp
    src/Domain/Product/Product.hpp
    src/Domain/Product/Product.cpp
    src/Domain/Ports/IMessage.hpp
    src/Domain/Ports/IRepository.hpp
    src/Adapters/Repository/TrackingRepository.hpp
    src/Adapters/Repository/TrackingRepository.cpp
    src/Adapters/Repository/SqlRepository.hpp
    src/Adapters/Repository/SqlRepository.cpp
    src/Adapters/Notification/EmailSenderStub.hpp
    src/ServiceLayer/UoW/AbstractUnitOfWork.cpp
    src/ServiceLayer/UoW/AbstractUnitOfWork.hpp
    src/ServiceLayer/UoW/IUnitOfWork.hpp
    src/ServiceLayer/UoW/SqlUnitOfWork.cpp
    src/ServiceLayer/UoW/SqlUnitOfWork.hpp
    src/ServiceLayer/MessageBus/Handlers/Handlers.cpp
    src/ServiceLayer/MessageBus/Handlers/Handlers.hpp
    src/ServiceLayer/MessageBus/MessageBus.cpp
    src/ServiceLayer/Views.hpp
    src/ServiceLayer/Views.cpp
    src/ServiceLayer/CommandFactory.hpp
    src/ServiceLayer/CommandFactory.cpp
    src/Entrypoints/REST/AddBatchHandler.hpp
    src/Entrypoints/REST/AddBatchHandler.cpp
    src/Entrypoints/REST/AllocateHandler.hpp
    src/Entrypoints/REST/AllocateHandler.cpp
    src/Entrypoints/REST/AllocationsViewHandler.hpp
    src/Entrypoints/REST/AllocationsViewHandler.cpp
)
target_precompile_headers(${PROJECT_NAME}_objs PUBLIC ${CMAKE_SOURCE_DIR}/src/Precompile.hpp)

userver_add_sql_library(
    ${PROJECT_NAME}_sql
    NAMESPACE
    Allocation
    SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Adapters/Repository/queries
    OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    SQL_FILES
    *.sql
)

file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/schemas/*.yaml)
userver_target_generate_chaotic(
    ${PROJECT_NAME}-chgen
    # Generate serializers for responses
    GENERATE_SERIALIZERS
    # Map '/components/schemas/*' JSONSchema types to C++ types in 'Allocation::STEM' namespace
    LAYOUT "/components/schemas/([^/]*)/=Allocation::{stem}::{0}"
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/src
    SCHEMAS ${SCHEMAS}
    RELATIVE_TO ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}_objs
    PUBLIC
    ${PROJECT_NAME}_sql
    ${PROJECT_NAME}-chgen
    userver::core
)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# Unit Tests
add_executable(
    ${PROJECT_NAME}_unittest
    src/Tests/Unit/Batches.cpp
    src/Tests/Unit/Products.cpp
    src/Tests/Unit/Handlers.cpp
    #src/Tests/Utilities/Common.cpp
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)

# Integrations Tests
#add_executable(
#    ${PROJECT_NAME}_integrationtest
#    src/Tests/Integration/PgFixture.hpp
#    src/Tests/Integration/Repository.cpp
    #src/Tests/Integration/UnitOfWork_test.cpp
    #src/Tests/Unit/Handlers.cpp
    #src/Tests/Utilities/Common.cpp
#)
#target_link_libraries(${PROJECT_NAME}_integrationtest PRIVATE ${PROJECT_NAME}_objs userver::utest)
#add_google_tests(${PROJECT_NAME}_integrationtest)


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