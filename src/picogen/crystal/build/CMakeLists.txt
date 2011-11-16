cmake_minimum_required (VERSION 2.6)
project (crystal)

set(CMAKE_BUILD_TYPE Release)

if (CMAKE_COMPILER_IS_GNUCXX)
        add_definitions (# language
                         -std=c++0x

                         # optimization
                         -O3
                         -ffast-math
                         -msse -mfpmath=sse
                         -Wdisabled-optimization
                         -march=native

                         # diagnosis
                         -g

                         # warnings
                         -Wall -Wextra -D__STDC_LIMIT_MACROS)

        #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -p -fprofile-generate")
        #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-use")

endif()


# -- openmp package -----------------------------------------------------------
find_package (OpenMP REQUIRED)
link_libraries(gomp)
add_definitions (${OpenMP_CXX_FLAGS})




# -- packages ---------------------------------------------------------------
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

### QT ###
find_package(Qt4 REQUIRED)

# Include dirs.
include_directories (
        ./                   # for generated stuff
        ../                  # project source folder
        ${QT_INCLUDE_DIR}    # self-explaining
)

# Used Qt Modules.
#set(QT_USE_QTWEBKIT TRUE)

include(${QT_USE_FILE})



### BOOST ###
find_package(Boost COMPONENTS
                   #program_options
                   #system
                   #filesystem
                   REQUIRED)
link_libraries(
  #${Boost_FILESYSTEM_LIBRARY}
  #${Boost_SYSTEM_LIBRARY}
  #${Boost_PROGRAM_OPTIONS_LIBRARY}
)


### OpenEXR ###
#find_package (OpenEXR REQUIRED )
#link_libraries (${OPENEXR_LIBRARIES})
#include_directories (${OPENEXR_INCLUDE_PATHS})

### LibNoise ###
#find_package (Noise REQUIRED)
#link_libraries(${NOISE_LIBRARY})
#include_directories(${NOISE_INCLUDE_DIR})

### SDL ###
#find_package (SDL REQUIRED )
#find_package (SDL_image REQUIRED )
#if (NOT SDL_FOUND )
#   message ( FATAL_ERROR "SDL not found!" )
#endif (NOT SDL_FOUND )

#link_libraries (
#   ${SDL_LIBRARY}
#   ${SDLIMAGE_LIBRARY} # if using SDL_image, obviously
#   SDLmain # Sadly not included in SDL_LIBRARY variable
#)
#include_directories(${SDL_INCLUDE_DIR})



# -- subs ---------------------------------------------------------------------

# -- sources ------------------------------------------------------------------
set (crystal_src_dir ../src)
include_directories (${crystal_src_dir})
set (sources ${sources}
        ${crystal_src_dir}/main.cc
        ${crystal_src_dir}/mainwindow.cc
        ${crystal_src_dir}/renderwidget.cc

        ${crystal_src_dir}/film.cc

        ${crystal_src_dir}/cameras/pinhole.cc
)



# -- ui -----------------------------------------------------------------------
set(forms ${forms}
        ${crystal_src_dir}/mainwindow.ui
        ${crystal_src_dir}/renderwidget.ui
)

#qt4_add_resources(resources "${crystal_src_dir}/resources.qrc")
set (sources ${sources} ${resources})

qt4_automoc (${sources})
qt4_wrap_ui (forms_h ${forms})
link_libraries (${QT_LIBRARIES})





# -- targets ------------------------------------------------------------------
add_executable (crystal  ${sources} ${forms_h})