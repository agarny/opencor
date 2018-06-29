project(libCellMLPlugin)

# Name and version of our package

set(PACKAGE_NAME libCellML)
set(PACKAGE_VERSION 0.1.0)

# Version of our package's libraries

set(LIBRARY_VERSION 0.1.0)

# Git tag for our library

set(GIT_TAG opencor)

# Package repository and release tag

set(PACKAGE_REPOSITORY libcellml)
set(RELEASE_TAG opencor)

# Specify where our local package will be installed

set(FULL_LOCAL_EXTERNAL_PACKAGE_DIR ${PROJECT_SOURCE_DIR}/${LOCAL_EXTERNAL_PACKAGE_DIR})

# Name of our package's shared library (and of its import library, if any)

if(WIN32)
    if(RELEASE_MODE)
        set(DEBUG_TAG)
    else()
        set(DEBUG_TAG d)
    endif()

    set(SHARED_LIBRARY libcellml${DEBUG_TAG}${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(IMPORT_LIBRARY cellml${DEBUG_TAG}${CMAKE_IMPORT_LIBRARY_SUFFIX})
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
                                  ${RELATIVE_PROJECT_SOURCE_DIR} 0fdd68d071b4d12f0d9b2265f0b188ed70af5958
                                  PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                                  RELEASE_TAG ${RELEASE_TAG}
                                  SHA1_FILES ${SHA1_FILES}
                                  SHA1_VALUES a45c504d8de06cb9b43e4a5604d50172dbc53726
                                              1e5754e29de00339f3b91d58d7b8ca9a01671eff)
        else()
            retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                                  ${RELATIVE_PROJECT_SOURCE_DIR} 4aae04bf5b95835d81137b5ad52ce3a4b5ba2ccf
                                  PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                                  RELEASE_TAG ${RELEASE_TAG}
                                  SHA1_FILES ${SHA1_FILES}
                                  SHA1_VALUES ec9954ed24f57262d88917069008bf46418f5e6d
                                              ae099f8614867e941a7ed96ef233519c0b285b55)
        endif()
    elseif(APPLE)
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${RELATIVE_PROJECT_SOURCE_DIR} 530b98eff20e1c662cdbf93fe4925d5243bf994d
                              PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                              RELEASE_TAG ${RELEASE_TAG}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES 27dc7c08c4103fa3a8230a7e83272b3acff2a4a9)
    else()
        retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                              ${RELATIVE_PROJECT_SOURCE_DIR} 477f4e4b28539af7fa3929700768ab105afac01c
                              PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                              RELEASE_TAG ${RELEASE_TAG}
                              SHA1_FILES ${SHA1_FILES}
                              SHA1_VALUES cb5b28ed938d7bbf64101e0d80ab5ba754c3708c)
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

    if(WIN32)
        # We want both our shared and import libraries to be in the same place

        ExternalProject_Add_Step(${PACKAGE_BUILD} copyDll
                                 COMMAND ${CMAKE_COMMAND} -E copy bin/${SHARED_LIBRARY} lib
                                 WORKING_DIRECTORY ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}
                                 DEPENDEES install)
    endif()

    # Package our external project

    create_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                        ${LOCAL_EXTERNAL_PACKAGE_DIR}
                        PACKAGE_REPOSITORY ${PACKAGE_REPOSITORY}
                        RELEASE_TAG ${RELEASE_TAG}
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
