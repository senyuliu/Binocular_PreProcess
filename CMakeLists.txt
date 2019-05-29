project(Binocular_PreProcess)

cmake_minimum_required(VERSION 3.5)

#######################OPENCV#############
FIND_PACKAGE(OpenCV REQUIRED)

add_compile_options(-fPIC)
add_compile_options(-std=c++11)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

####################QT#####################
find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
include_directories(preprocess)
include_directories(src)

add_definitions(-DQT_NO_KEYWORDS)

qt5_wrap_cpp(MOC_FILES src/myviz.h)

#add_executable(display src/myviz.h src/myviz.cpp src/smartc_show.cpp src/Dialog.h src/Dialog.cpp $MOC_FILES)

###########################thread###########

find_package(Threads)

add_executable(Binocular_PreProcess  ${MOC_FILES} src/myviz.h src/myviz.cpp src/smartc_show.cpp src/Dialog.h src/Dialog.cpp preprocess/GPS.cpp preprocess/shellCommand.cpp preprocess/cJSON.c preprocess/StereoRectify.cpp preprocess/DataParse.cpp)


target_link_libraries(Binocular_PreProcess ${OpenCV_LIBS} ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} glog ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})


