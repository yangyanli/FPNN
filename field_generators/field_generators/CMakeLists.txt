set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(Boost_USE_STATIC_LIBS  ON)
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED system thread filesystem)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIRS})

#   Google-gflags
find_package(GFlags REQUIRED)
include_directories(${GFLAGS_INCLUDE_DIRS})

find_package(HDF5 REQUIRED CXX)
include_directories(${HDF5_INCLUDE_DIRS})
link_directories(${HDF5_LIBRARY_DIRS})

find_package(PCL REQUIRED common io kdtree search filters features)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_common_LIBRARY_DIRS})
link_directories(${PCL_io_LIBRARY_DIRS})
link_directories(${PCL_kdtree_LIBRARY_DIRS})
link_directories(${PCL_search_LIBRARY_DIRS})
link_directories(${PCL_filters_LIBRARY_DIRS})
link_directories(${PCL_features_LIBRARY_DIRS})
add_definitions(${PCL_common_DEFINITIONS})
add_definitions(${PCL_io_DEFINITIONS})
add_definitions(${PCL_kdtree_DEFINITIONS})
add_definitions(${PCL_search_DEFINITIONS})
add_definitions(${PCL_filters_DEFINITIONS})
add_definitions(${PCL_features_DEFINITIONS})

find_package(Flann REQUIRED)
include_directories(${FLANN_INCLUDE_DIRS})

find_package(CGAL REQUIRED)
include_directories(${CGAL_INCLUDE_DIRS})
link_directories(${CGAL_LIBRARIES_DIR})

find_package(OpenSceneGraph REQUIRED osgViewer osgText osgDB osgGA osgQt osgManipulator osgUtil)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/mesh_io/include)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set Qt files and resources here
set(uis       resources/main_window.ui)
set(resources  resources/main_window.qrc)

# Qt stuff
QT5_WRAP_UI(ui_srcs ${uis})
QT5_ADD_RESOURCES(resource_srcs ${resources})

file(GLOB incs "./include/*.h*")
file(GLOB srcs "./src/*.cpp*")
file(GLOB impl_incs "include/impl/*.h*")

# Organize files
SOURCE_GROUP("Resources" FILES ${uis} ${resources})
SOURCE_GROUP("Generated" FILES ${ui_srcs} ${moc_srcs} ${resource_srcs})
SET_SOURCE_FILES_PROPERTIES(${srcs} PROPERTIES OBJECT_DEPENDS "${ui_srcs}")

# Put the ui in the windows project file
IF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
  SET (srcs ${srcs} ${uis})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
IF (${CMAKE_BUILD_TOOL} MATCHES "devenv")
  SET (srcs ${srcs} ${uis})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "devenv")

set(exe_name field_generators)
add_executable(${exe_name} ${ui_srcs} ${moc_srcs} ${resource_srcs} ${srcs} ${incs} ${impl_incs})
target_link_libraries(${exe_name} mesh_io
  ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}
  ${GLOG_LIBRARIES}
  ${GFLAGS_LIBRARIES}
  ${HDF5_CXX_LIBRARIES}
  ${CGAL_LIBRARY} ${CGAL_CORE_LIBRARY}
  ${PCL_COMMON_LIBRARY} ${PCL_IO_LIBRARY}
  ${PCL_KDTREE_LIBRARY} ${PCL_SEARCH_LIBRARY}
  ${PCL_FILTERS_LIBRARY} ${PCL_FEATURES_LIBRARY}
  ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${OSGGA_LIBRARY}
  ${OSGMANIPULATOR_LIBRARY} ${OSGQT_LIBRARY} ${OSGTEXT_LIBRARY}
  ${OSGUTIL_LIBRARY} ${OSGVIEWER_LIBRARY}
  Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Xml Qt5::Concurrent
)

if(WIN32 AND MSVC)
  set_target_properties(${exe_name} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
  set_target_properties(${exe_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
elseif(CMAKE_SYSTEMname STREQUAL "Darwin")
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -Wl)
  endif()
elseif(__COMPILER_PATHSCALE)
  set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -mp)
endif()

set_target_properties(${exe_name} PROPERTIES DEBUG_POSTFIX _debug)
set_target_properties(${exe_name} PROPERTIES RELEASE_POSTFIX _release)
