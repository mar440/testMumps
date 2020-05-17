cmake_minimum_required(VERSION 3.0)

project(testMumps)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/") 

#file(GLOB sources
#  "${PROJECT_SOURCE_DIR}/include/*.hpp",
#  "${PROJECT_SOURCE_DIR}/src/*.cpp")


if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAdd_ -Wall  -std=c++11 -fPIC -Wunused-but-set-variable")
endif()
#-lstdc++


#################
## THREADS ######
#################
find_package(Threads)

#################
## MKL ##########
#################
find_package(MKL REQUIRED )
find_package(Threads)

include_directories(${MKL_INCLUDE_DIR})
link_directories(${MKL_LIBRARY_DIR})

set(BLAS_VENDOR Intel10_64lp) 
find_package(BLAS REQUIRED) 
find_package(LAPACK REQUIRED)

set(MKL_COMPILER_LIB_DIR ${MKL_COMPILER_LIB_DIR} m dl pthread mkl_core mkl_sequential)
set(MKL_LIB_DIR ${MKL_LIB_DIR} m dl mkl_sequential mkl_core pthread)

# #################
# ## MPI ##########
# #################
# find_package(MPI REQUIRED)
# # add this line only when you are using openmpi which has a different c++ bindings
# add_definitions(-DOMPI_SKIP_MPICXX)

#################
## LAPACK #######
#################
find_package(LAPACK)

#################
## METIS ########
#################
set(MUMPS_DIR $ENV{MUMPS_DIR})
set(METIS_LIBRARIES $ENV{METIS_LIBRARIES}/libmetis.a CACHE PATH "mydoc")
set(METIS_INCLUDEDIRS $ENV{METIS_INCLUDEDIRS} CACHE PATH "mydoc")
include_directories(${METIS_INCLUDEDIRS} ${CMAKE_CURRENT_SOURCE_DIR})

#################
## FORTRAN ######
#################
enable_language(Fortran)

#################
## MUMPS ########
#################
include_directories("${MUMPS_DIR}/include")
link_directories("${MUMPS_DIR}/lib" "${MUMPS_DIR}/libseq")

######################################################################################
add_executable(${PROJECT_NAME} main.cpp ) #${sources})


target_link_libraries(${PROJECT_NAME}
  ${METIS_LIBRARIES} ${BLAS_LIBRARIES}
  libdmumps.a libmumps_common.a libpord.a libmpiseq.a
  -lesmumps -lscotch -lscotcherr
  -lmetis
   -fopenmp gfortran)


set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
