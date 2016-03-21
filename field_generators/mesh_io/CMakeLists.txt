set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_definitions(-DYY_NO_UNISTD_H)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/PLY)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(MtlParser 
             src/mtl.y 
             ${CMAKE_CURRENT_BINARY_DIR}/mtlparser.y.c)

FLEX_TARGET(MtlScanner 
            src/mtl.l
            ${CMAKE_CURRENT_BINARY_DIR}/mtlscanner.l.c)

BISON_TARGET(ObjParser 
             src/wavefrontobj.y 
             ${CMAKE_CURRENT_BINARY_DIR}/objparser.y.c)

FLEX_TARGET(ObjScanner 
            src/wavefrontobj.l
            ${CMAKE_CURRENT_BINARY_DIR}/objscanner.l.c)

ADD_FLEX_BISON_DEPENDENCY(MtlScanner MtlParser)
ADD_FLEX_BISON_DEPENDENCY(ObjScanner ObjParser)

set(incs    include/mesh_io_exports.h
            include/mesh_io.h
            include/usercallbacks.h
            include/MtlContext.h
            include/ObjContext.h
            include/OffContext.h
            include/PLY/base.h
            include/PLY/header.h
            include/PLY/io.h
            include/PLY/object.h
            include/PLY/unknown.h
            )

set(srcs    src/off_reader.cpp
            src/PLY/base.cpp
            src/PLY/header.cpp
            src/PLY/io.cpp
            src/PLY/object.cpp
            src/PLY/unknown.cpp)

set(yacc_files  src/mtl.y
                src/wavefrontobj.y)
set(lex_files   src/mtl.l
                src/wavefrontobj.l)

source_group("Yacc Files" FILES ${yacc_files})
source_group("Lex Files" FILES ${lex_files})
add_custom_target(lex-yacc SOURCES ${yacc_files} ${lex_files})

set(generated_files ${BISON_MtlParser_OUTPUTS}
                    ${FLEX_MtlScanner_OUTPUTS}
                    ${BISON_ObjParser_OUTPUTS}
                    ${FLEX_ObjScanner_OUTPUTS})

set(lib_name mesh_io)
add_library(${lib_name} ${generated_files} ${incs} ${srcs})

if(WIN32 AND MSVC)
  set_target_properties(${lib_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
elseif(CMAKE_SYSTEMname STREQUAL "Darwin")
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set_target_properties(${lib_name} PROPERTIES LINK_FLAGS -Wl)
  endif()
elseif(__COMPILER_PATHSCALE)
  set_target_properties(${lib_name} PROPERTIES LINK_FLAGS -mp)
else()
  set_target_properties(${lib_name} PROPERTIES LINK_FLAGS -Wl)
endif()

set_target_properties(${lib_name} PROPERTIES DEFINE_SYMBOL "MESH_IO_API_EXPORTS")

set_target_properties(${lib_name} PROPERTIES DEBUG_POSTFIX _debug)
set_target_properties(${lib_name} PROPERTIES RELEASE_POSTFIX _release)
