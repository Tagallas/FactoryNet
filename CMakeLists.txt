cmake_minimum_required(VERSION 3.21)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        #src/factory.cpp
        src/helpers.cpp
        src/nodes.cpp
        )

add_executable(NetSim ${SOURCE_FILES} main.cpp)

# == Unit testing using Google Testing Framework ==

# Ustaw zmienną `SOURCES_FILES_TESTS`, która będzie przechowywać ścieżki do
# plików źródłowych z testami jednostkowymi.
set(SOURCES_FILES_TESTS
        test/test_package.cpp
        test/test_storage_types.cpp
        test/test_nodes.cpp
        #test/test_Factory.cpp
        #test/test_factory_io.cpp
        #test/test_reports.cpp
        #test/test_simulate.cpp
        )

# Dodaj konfigurację typu `Test`.
add_executable(my_project_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

target_compile_definitions(my_project_test PUBLIC EXERCISE_ID=EXERCISE_ID_NODES)


# Dodaj katalogi z plikami nagłówkowymi wyłącznie do konkretnej konfiguracji (tu: `Test`).
target_include_directories(my_project_test PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
        )

# Dodaj podkatalog `googletest-master`.
add_subdirectory(googletest-master)

# Podlinkuj bibliotekę o identyfikatorze `gmock` (w pliku CMake) wyłącznie do konkretnej
# konfiguracji (tu: `Test`).
target_link_libraries(my_project_test gmock)