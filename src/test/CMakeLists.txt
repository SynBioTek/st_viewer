###############################################################################
# Unit Test CMake                                                             #
###############################################################################

use_qt5lib(Qt5Test)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/QtUnitTest ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/src ${CMAKE_BINARY_DIR})

set(TARGET_NAME stVi_test)

set(ST_MAIN
    controller/data/tst_datastoretest.cpp
    controller/data/tst_datastoretest.h
    controller/tst_widgets.cpp
    controller/tst_widgets.h
    model/tst_objectparsertest.cpp
    model/tst_objectparsertest.h
    utils/tst_mathextendedtest.cpp
    utils/tst_mathextendedtest.h
)

set(LIB_ST_SIMPLECRYPT
    simpleCrypt/tst_simplecryptdevicetest.cpp
    simpleCrypt/tst_simplecryptdevicetest.h
    simpleCrypt/tst_simplecrypttest.cpp
    simpleCrypt/tst_simplecrypttest.h
)

set(LIB_ST_NETWORK
    network/test_auth.cpp
    network/test_rest.cpp
)

set(LIB_ST_MATH
    math/tst_glaabbtest.cpp
    math/tst_glaabbtest.h
    math/tst_glquadtreetest.cpp
    math/tst_glquadtreetest.h
    math/tst_glheatmaptest.cpp
    math/tst_glheatmaptest.h
)

# Define source files
set(ST_UNITTEST_SOURCES 
    unit_main.cpp
    ${ST_MAIN}
    ${LIB_ST_SIMPLECRYPT}
    ${LIB_ST_MATH}
    ${LIB_ST_NETWORK}
    $<TARGET_OBJECTS:QtUnitTest>
    ${ST_TARGET_OBJECTS}
)
 
# add executable

if(WIN32)
    add_executable(${TARGET_NAME} WIN32 ${ST_UNITTEST_SOURCES})
    target_link_libraries(${TARGET_NAME} ${Qt5Core_QTMAIN_LIBRARIES}) #Note: allows compile windows application (ie. not console)
else()
    add_executable(${TARGET_NAME} ${ST_UNITTEST_SOURCES})
endif()

target_link_libraries(${TARGET_NAME} ${LIB_COMPONENTS} ${QT_TARGET_LINK_LIBS} ${OPENGL_LIBRARY} QtDebugHelper) #STCoreWidgets
qt5_use_modules(${TARGET_NAME} Test)

add_test(general ${TARGET_NAME})

add_custom_target(unittest ${TARGET_NAME})