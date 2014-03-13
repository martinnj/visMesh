#------------------------------------------------------------
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0" "1.46.1")

#------------------------------------------------------------
SET(
  CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
  ${CMAKE_MODULE_PATH}
  )

SET(CMAKE_FIND_FRAMEWORK "LAST")
FIND_PACKAGE(MAYA)
#FIND_PACKAGE(OPENGL)
FIND_PACKAGE(LAPACK)

# The following lines are needed to find the DSC project files for proper linking.
#SET(DSC_PROJECT_DIR "D:/Dropbox/4th year/Blok 1 - BSc/dsc-repo/CODE/DSC_PROJECT")
#FIND_PACKAGE(DSC_PROJECT)
#------------------------------------------------------------
SET(MAYA_PLUGIN_NAME "visMesh")

# Make the compiler build plugin directly into Mayas plugin dir.
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${MAYA_BASE_DIR}/bin/plug-ins )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${MAYA_BASE_DIR}/bin/plug-ins )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${MAYA_BASE_DIR}/bin/plug-ins )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )


FIND_PACKAGE(3RDPARTY     PATHS ${3RDPARTY}       REQUIRED)
FIND_PACKAGE(GEL          PATHS ${GEL}            REQUIRED)

#-----------------------------------------------------------
FILE(
  GLOB
  MAYA_PLUGIN_SOURCES
  ${PROJECT_SOURCE_DIR}/PLUGIN/src/*.cpp
  )

FILE(
  GLOB
  MAYA_PLUGIN_HEADERS
  ${PROJECT_SOURCE_DIR}/PLUGIN/include/*.h
  )

  
## Give me dem header files :)
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/PLUGIN/include
  ${MAYA_INCLUDE_DIRS}
  ${3RDPARTY_INCLUDE_DIRS}
  ${GEL_INCLUDE_DIRS}
  ${DSC_DIR}/DSC/include
  ${DSC_DIR}/CHANVESE/include
  )

ADD_LIBRARY(
  ${MAYA_PLUGIN_NAME}
  SHARED
  ${MAYA_PLUGIN_SOURCES}
  ${MAYA_PLUGIN_HEADERS}
  )

## Paths to BLAS and LAPACK, these are needed for DSC, not needed anywhere else.
## So can be removed :)
SET( MESSYFILES
     "C:/lapack/BLAS.lib"
     "C:/lapack/LAPACK.lib"
)

## Link it all together.
TARGET_LINK_LIBRARIES(
  ${MAYA_PLUGIN_NAME}
  ${MAYA_Foundation_LIBRARY}
  ${MAYA_OpenMaya_LIBRARY}
  ${MAYA_OpenMayaUI_LIBRARY}
  ${MAYA_OpenMayaRender_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  #${3RDPARTY_LIBRARIES}
  #${GEL_LIBRARIES}
  ${LAPACK_LIBRARIES}
  ${MESSYFILES}
  ${PROJECT_SOURCE_DIR}/PLUGIN/lib/dsc.lib
  #${PROJECT_SOURCE_DIR}/PLUGIN/lib/dscD.lib
  ${PROJECT_SOURCE_DIR}/PLUGIN/lib/chanvese_lib.lib
  #${PROJECT_SOURCE_DIR}/PLUGIN/lib/chanvese_libD.lib
  )

#--------------------------------------------------------

# Definitions that is needed in order to compile the Maya headers.
IF(WIN32)
  SET(
    MAYA_DEFINITIONS
    _AFXDLL
    _MBCS
    NT_PLUGIN
    REQUIRE_IOSTREAM
    _BOOL
    )
ELSEIF(APPLE)
  SET(
    MAYA_DEFINITIONS
    MAC_PLUGIN
    OSMac_MachO_  
    UNIX 
    _BOOL 
    OSMac_
    REQUIRE_IOSTREAM
    )
ENDIF()

SET_TARGET_PROPERTIES(
  ${MAYA_PLUGIN_NAME}
  PROPERTIES
  COMPILE_DEFINITIONS
  "${MAYA_DEFINITIONS}"
  )

SET_TARGET_PROPERTIES(
  ${MAYA_PLUGIN_NAME}
  PROPERTIES
  OUTPUT_NAME
  "${MAYA_PLUGIN_NAME}"
  )

# The suffix might need to be changed if on Linux, note sure yet.
SET_TARGET_PROPERTIES(${MAYA_PLUGIN_NAME} PROPERTIES SUFFIX ".mll")
SET_TARGET_PROPERTIES(${MAYA_PLUGIN_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

IF(WIN32)

  SET_TARGET_PROPERTIES(
    ${MAYA_PLUGIN_NAME}
    PROPERTIES
    LINK_FLAGS
    "/export:initializePlugin /export:uninitializePlugin"
    )

ENDIF()

#------------------------------------------------------------------
SET_TARGET_PROPERTIES(${MAYA_PLUGIN_NAME} PROPERTIES DEBUG_POSTFIX "D" )

#------------------------------------------------------------------
SET_TARGET_PROPERTIES(${MAYA_PLUGIN_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_SOURCE_DIR}/lib")
SET_TARGET_PROPERTIES(${MAYA_PLUGIN_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_SOURCE_DIR}/lib")
SET_TARGET_PROPERTIES(${MAYA_PLUGIN_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_SOURCE_DIR}/lib")
