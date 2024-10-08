# [[ Project ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]

# Each internal library can act as a project. You can also just use a normal
# variable name for the same purpose if you wish to.

# Project name cannot contain any spaces. Use underlines instead.
project("Lib_Discoverer"

    VERSION 0.0.1

    DESCRIPTION "A simple utility for logging messages."

    LANGUAGES CXX)

# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ Project ]]





# [[ Files ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]

set(HEADER_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/include/discoverer.hpp")
	

set(SOURCE_FILES

    "${CMAKE_CURRENT_SOURCE_DIR}/src/discoverer.cpp")

# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ Files ]]






# [[ Library ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]

qt_add_library(${PROJECT_NAME} STATIC)




target_sources(${PROJECT_NAME}

    PUBLIC
    ${HEADER_FILES}

    PRIVATE
    ${SOURCE_FILES})









target_link_libraries(${PROJECT_NAME}

    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::Sql
    Qt6::Multimedia
    Qt6::Bluetooth)

# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ Library ]]



find_package(Threads REQUIRED)


include(FetchContent)

FetchContent_Declare(asio
  GIT_REPOSITORY "https://github.com/chriskohlhoff/asio.git"
  GIT_TAG master
  GIT_SHALLOW TRUE)


FetchContent_MakeAvailable(asio)


target_include_directories(${PROJECT_NAME}

    PUBLIC

    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${FETCHCONTENT_BASE_DIR}/asio-src/asio/include/")


# [[ Internal Library ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]

# Add any other internal libraries that this internal library may depend on.

target_link_libraries(${PROJECT_NAME}

    PRIVATE)

# [[ ----------------------------------------------------------------------- ]]
# [[ ----------------------------------------------------------------------- ]]
# [[ Internal Library ]]
