cmake_minimum_required(VERSION 3.14)

project(xml2json LANGUAGES CXX)

set(WITH_QT ON)

if(${WITH_QT})

    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(${WITH_QT})
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Xml REQUIRED)

    add_definitions(-DWITH_QT)
else()
    find_package(Poco REQUIRED PocoXml)
    find_package(Poco REQUIRED PocoJson)
endif()

add_executable(xml2json
  main.cpp
  xml2jsonqt.h
  xml2jsonqt.cpp
)

if(${WITH_QT})
    target_link_libraries(xml2json Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Xml)
else()
    target_link_libraries(xml2json )
endif()
