project(libCellMLPlugin)

# Name and version of our package

set(PACKAGE_NAME libCellML)
set(PACKAGE_VERSION 0.1.0)

# Version of our package's libraries

set(LIBRARY_VERSION 0.1.0)

# Git tag for our library

set(GIT_TAG ec0cc06)

# Package repository

set(PACKAGE_REPOSITORY libcellml)

# Specify where our local package will be installed

set(FULL_LOCAL_EXTERNAL_PACKAGE_DIR ${PROJECT_SOURCE_DIR}/${LOCAL_EXTERNAL_PACKAGE_DIR})

# Name of our package's shared library (and of its import library, if any)

if(WIN32)
    set(SHARED_LIBRARY cellml${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(IMPORT_LIBRARY cellml${CMAKE_IMPORT_LIBRARY_SUFFIX})
else()
    set(SHARED_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}cellml${CMAKE_SHARED_LIBRARY_SUFFIX_PRE}.${LIBRARY_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX_POST})
    set(IMPORT_LIBRARY ${SHARED_LIBRARY})
endif()

# Retrieve our package's binaries

set(EXTERNAL_BINARIES_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/lib)

set(EXTERNAL_BINARIES
    ${SHARED_LIBRARY}
)

# Specify the files that need to have their SHA-1 value checked

set(SHA1_FILES
    lib/${SHARED_LIBRARY}
)

if(WIN32)
    list(APPEND SHA1_FILES
        lib/${IMPORT_LIBRARY}
    )
endif()

# Use the pre-built version of our package unless instructed otherwise

if(USE_PREBUILT_LIBCELLML_PACKAGE)
    # Retrieve the plugin's package

    string(REPLACE "${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/" ""
           RELATIVE_PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/${REMOTE_EXTERNAL_PACKAGE_DIR}")

    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                  ${RELATIVE_PROJECT_SOURCE_DIR} XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                                  PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                                  SHA1_FILES ${SHA1_FILES}
                                  SHA1_VALUES XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                                              XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX)
        else()
            retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                  ${RELATIVE_PROJECT_SOURCE_DIR} XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                                  PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                                  SHA1_FILES ${SHA1_FILES}
                                  SHA1_VALUES XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                                              XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX)
        endif()
    elseif(APPLE)
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${RELATIVE_PROJECT_SOURCE_DIR} XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                              PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX)
    else()
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${RELATIVE_PROJECT_SOURCE_DIR} XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                              PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX)
    endif()
else()
    # Build libCellML as an external project

    set(PACKAGE_BUILD ${PACKAGE_NAME}Build)

    ExternalProject_Add(${PACKAGE_BUILD}
        SOURCE_DIR
            ${CMAKE_SOURCE_DIR}/ext/${PACKAGE_NAME}
        INSTALL_DIR
            ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}
        GIT_REPOSITORY
            https://github.com/opencor/libcellml
        GIT_TAG
            ${GIT_TAG}
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DLIBCELLML_BINDINGS_PYTHON=OFF
            -DLIBCELLML_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DLIBCELLML_COVERAGE=OFF
            -DLIBCELLML_INSTALL_PREFIX:PATH=<INSTALL_DIR>
            -DLIBCELLML_TREAT_WARNINGS_AS_ERRORS=OFF
            -DLIBCELLML_UNIT_TESTS=OFF
        BUILD_BYPRODUCTS
            <INSTALL_DIR>/lib/${IMPORT_LIBRARY}
    )

    # Package our external project

    create_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                        ${LOCAL_EXTERNAL_PACKAGE_DIR}
                        PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                        PACKAGED_FILES include ${SHA1_FILES}
                        SHA1_FILES ${SHA1_FILES}
                        TARGET ${PACKAGE_BUILD})

    set(DEPENDS_ON ${PACKAGE_BUILD})
endif()

# Allow other external projects to find our libCellML package

set(LIBCELLML_ROOT_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} PARENT_SCOPE)
set(LIBCELLML_INCLUDE_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/include PARENT_SCOPE)
set(LIBCELLML_LIBRARY ${EXTERNAL_BINARIES_DIR}/${IMPORT_LIBRARY} PARENT_SCOPE)

# Add the plugin

add_plugin(${PACKAGE_NAME}
    SOURCES
        ../../plugininfo.cpp

        src/libcellmlplugin.cpp
    HEADERS_MOC
        src/libcellmlplugin.h
    QT_MODULES
        Core
    EXTERNAL_BINARIES_DIR
        ${EXTERNAL_BINARIES_DIR}
    EXTERNAL_BINARIES
        ${EXTERNAL_BINARIES}
    TESTS
        tests
    DEPENDS_ON
        ${DEPENDS_ON}
)
