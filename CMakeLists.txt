cmake_minimum_required(VERSION 3.14)

project(xml2json LANGUAGES CXX)

# using qt
set(WITH_QT CACHE BOOL OFF)
# using poco (not yet realized)
set(WITH_POCO CACHE BOOL OFF)

if(${WITH_QT})

    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBS "")

if(${WITH_QT})
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Xml REQUIRED)

    add_definitions(-DWITH_QT)
else()
    find_package(jsoncpp REQUIRED)
    if(${WITH_POCO})
        find_package(Poco REQUIRED XML)
        find_package(Poco REQUIRED JSON)
        add_definitions(-DWITH_POCO)
        include_directories(${POCO_INCLUDE_DIR})
        message(STATUS "poco " ${POCO_INCLUDE_DIR})
    else()
        # by default using libxml2 and jsoncpp
        find_package(LibXml2 REQUIRED)
        set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})
        include_directories(${LIBXML2_INCLUDE_DIR})
        message(STATUS "libxml2 " ${LIBXML2_INCLUDE_DIR})
    endif()
    set(LIBS ${LIBS} -ljsoncpp)
endif()

add_executable(xml2json
  main.cpp
  common.h
  xml2jsonqt.h
  xml2jsonqt.cpp
  xml2jsonpoco.h
  xml2jsonpoco.cpp
  xml2jsonlibxml.h
  xml2jsonlibxml.cpp
)

if(${WITH_QT})
    target_link_libraries(xml2json Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Xml)
elseif(${WITH_POCO})
    target_link_libraries(xml2json PUBLIC Poco::XML ${LIBS})
else()
    target_link_libraries(xml2json ${LIBS})
endif()
