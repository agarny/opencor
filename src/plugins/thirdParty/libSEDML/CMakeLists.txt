project(libSEDMLPlugin)

# Name and version of our package

set(PACKAGE_NAME libSEDML)
set(PACKAGE_VERSION 2.0.6)

# Version of our package's libraries

set(MAJOR_LIBRARY_VERSION 2)
set(LIBRARY_VERSION ${MAJOR_LIBRARY_VERSION}.0.6)

# Git tag for our library

set(GIT_TAG l1v4)

# Package repository

set(PACKAGE_REPOSITORY libSEDML)

# Specify where our local package will be installed

string(REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/ext"
       FULL_LOCAL_EXTERNAL_PACKAGE_DIR "${PROJECT_SOURCE_DIR}/${EXTERNAL_PACKAGE_DIR}")

# Name of our package's shared library (and of its import library, if any)

if(WIN32)
    set(SHARED_LIBRARY libsedml${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(IMPORT_LIBRARY libsedml${CMAKE_IMPORT_LIBRARY_SUFFIX})
else()
    set(SHARED_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}sedml${CMAKE_SHARED_LIBRARY_SUFFIX_PRE}.${MAJOR_LIBRARY_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX_POST})
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

if(USE_PREBUILT_LIBSEDML_PACKAGE)
    # Retrieve the plugin's package

    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                  ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} e9c219ea7044cdc74d59c9758a77b470ee251cd8
                                  PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                                  SHA1_FILES ${SHA1_FILES}
                                  SHA1_VALUES 2424be8cc717663b4a5d45fc17bf948a3a78792b
                                              35c9e81d3a4b382a82c069cac4fb62fe10a09681)
        else()
            retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                  ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} d3c913f9b66187486e28e5fa86d9d67cee1faa0f
                                  PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                                  SHA1_FILES ${SHA1_FILES}
                                  SHA1_VALUES 98897b759e235e83bf06d1270217dbd28dab91bb
                                              35c9e81d3a4b382a82c069cac4fb62fe10a09681)
        endif()
    elseif(APPLE)
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} 7836dbf662333b56135a464fe5610050b0c9011d
                              PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES 68d7e6c1636ae4d1db70f1b7fd1bfa249d89ff73)
    else()
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} 3d7121ee7bbf613b41e299d8bb4d2c1944e1fd16
                              PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES d501d758455e446296382e2cb5b8a39ff36bb43c)
    endif()
else()
    # Build libSEDML as an external project
    # Note: we need both libSBML and libNuML to be built, so we can access them,
    #       as well as some of libSBML's dependencies...

    set(PACKAGE_BUILD ${PACKAGE_NAME}Build)

    if(WIN32)
        if(RELEASE_MODE)
            set(MODE release)
        else()
            set(MODE debug)
        endif()
    endif()

    ExternalProject_Add(${PACKAGE_BUILD}
        SOURCE_DIR
            ${CMAKE_SOURCE_DIR}/ext/${PACKAGE_NAME}
        INSTALL_DIR
            ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
            -DLIBNUML_INCLUDE_DIR=${LIBNUML_INCLUDE_DIR}
            -DLIBNUML_LIBRARY=${LIBNUML_LIBRARY}
            -DLIBSBML_INCLUDE_DIR=${LIBSBML_INCLUDE_DIR}
            -DLIBSBML_LIBRARY=${LIBSBML_LIBRARY}
            ${DEFINE_LIBZ_INCLUDE_DIR}
            ${DEFINE_LIBZ_LIBRARY}
            -DWITH_CPP_NAMESPACE=ON
            -DWITH_SWIG=OFF
            -DWITH_ZLIB=OFF
        BUILD_BYPRODUCTS
            <INSTALL_DIR>/lib/${IMPORT_LIBRARY}
        DEPENDS
            libNuMLPlugin
            libSBMLPlugin
    )

    # Do some post-processing that is specific to each platform

    if(WIN32)
        # We want both our shared and import libraries to be in the same place

        ExternalProject_Add_Step(${PACKAGE_BUILD} copyDll
                                 COMMAND ${CMAKE_COMMAND} -E copy bin/${SHARED_LIBRARY} lib
                                 WORKING_DIRECTORY ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}
                                 DEPENDEES install)
    else()
        # Dereference the symbolic link to the libraries since CMake's tar can't
        # do this

        ExternalProject_Add_Step(${PACKAGE_BUILD} copyLibrary
                                 COMMAND ${CMAKE_COMMAND} -E remove -f ${SHARED_LIBRARY}
                                 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SHARED_LIBRARY_PREFIX}sedml${CMAKE_SHARED_LIBRARY_SUFFIX_PRE}.${LIBRARY_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX_POST}
                                                                  ${SHARED_LIBRARY}
                                 WORKING_DIRECTORY ${EXTERNAL_BINARIES_DIR}
                                 DEPENDEES install)
    endif()

    # Package our external project

    create_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                        PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                        RELEASE_TAG ${RELEASE_TAG}
                        PACKAGED_FILES include ${SHA1_FILES}
                        SHA1_FILES ${SHA1_FILES}
                        TARGET ${PACKAGE_BUILD})

    set(DEPENDS_ON ${PACKAGE_BUILD})
endif()

# Allow other external projects to find our libSEDML package

set(LIBSEDML_ROOT_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} PARENT_SCOPE)
set(LIBSEDML_INCLUDE_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/include PARENT_SCOPE)
set(LIBSEDML_LIBRARY ${EXTERNAL_BINARIES_DIR}/${IMPORT_LIBRARY} PARENT_SCOPE)

# Add the plugin

add_plugin(${PACKAGE_NAME}
    SOURCES
        ../../plugininfo.cpp

        src/libsedmlplugin.cpp
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
