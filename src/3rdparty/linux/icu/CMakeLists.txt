project(ICU)

# Name and version of our package

set(PACKAGE_NAME ICU)
set(PACKAGE_VERSION 56.1)

# Version of our library

set(MAJOR_LIBRARY_VERSION 56)
set(LIBRARY_VERSION ${MAJOR_LIBRARY_VERSION}.1)

# Git tag for our library

set(GIT_TAG v56.1-centos7)

# Release tag

set(RELEASE_TAG v56.1-centos7)

# Specify where our local package will be installed

string(REGEX REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/ext"
       FULL_LOCAL_EXTERNAL_PACKAGE_DIR "${PROJECT_SOURCE_DIR}/${EXTERNAL_PACKAGE_DIR}")

# Name of our package's shared library

set(ICUDATA_SHARED_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}icudata${CMAKE_SHARED_LIBRARY_SUFFIX}.${MAJOR_LIBRARY_VERSION})
set(ICUI18N_SHARED_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}icui18n${CMAKE_SHARED_LIBRARY_SUFFIX}.${MAJOR_LIBRARY_VERSION})
set(ICUUC_SHARED_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}icuuc${CMAKE_SHARED_LIBRARY_SUFFIX}.${MAJOR_LIBRARY_VERSION})

# Specify the files that need to have their SHA-1 value checked

set(SHA1_FILES
    lib/${ICUDATA_SHARED_LIBRARY}
    lib/${ICUI18N_SHARED_LIBRARY}
    lib/${ICUUC_SHARED_LIBRARY}
)

# Use the pre-built version of our package unless instructed otherwise

if(USE_PREBUILT_ICU_PACKAGE)
    # Retrieve the library's package

    retrieve_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                          ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} bc06c8427b1da4da0031e42a5ba1b7c30be78c56
                          RELEASE_TAG ${RELEASE_TAG}
                          SHA1_FILES ${SHA1_FILES}
                          SHA1_VALUES 4941bffe14b2ad7ca226e52d8127d6b151a3c95d
                                      f767fe82c7f455e9fa5649e8c24577c663ea5d81
                                      95445c9b3d6156764e842d73704c511d3190046f)
else()
    # Build ICU as an external project

    set(PACKAGE_BUILD ${PACKAGE_NAME}Build)

    ExternalProject_Add(${PACKAGE_BUILD}
        SOURCE_DIR
            ${CMAKE_SOURCE_DIR}/ext/${PACKAGE_NAME}
        BINARY_DIR
            ${EXTERNAL_PROJECT_BUILD_DIR}/Build/${PACKAGE_BUILD}
        INSTALL_DIR
            ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}
        GIT_REPOSITORY
            https://github.com/opencor/icu
        GIT_TAG
            ${GIT_TAG}
        CONFIGURE_COMMAND
            <SOURCE_DIR>/source/runConfigureICU Linux --prefix=<INSTALL_DIR>
        BUILD_BYPRODUCTS
            <INSTALL_DIR>/lib/${ICUDATA_SHARED_LIBRARY}
            <INSTALL_DIR>/lib/${ICUI18N_SHARED_LIBRARY}
            <INSTALL_DIR>/lib/${ICUUC_SHARED_LIBRARY}
    )

    # Package our external project

    create_package_file(${PACKAGE_NAME} ${PACKAGE_VERSION}
                        PACKAGED_FILES include lib
                        SHA1_FILES ${SHA1_FILES}
                        TARGET ${PACKAGE_BUILD})
endif()

# Allow our main CMake project to find our ICU package

set(LOCAL_ICU_LIB_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR}/lib)

set(ICU_ROOT_DIR ${FULL_LOCAL_EXTERNAL_PACKAGE_DIR} PARENT_SCOPE)
set(ICU_ICUDATA_LIBRARY ${LOCAL_ICU_LIB_DIR}/${ICUDATA_SHARED_LIBRARY} PARENT_SCOPE)
set(ICU_ICUI18N_LIBRARY ${LOCAL_ICU_LIB_DIR}/${ICUI18N_SHARED_LIBRARY} PARENT_SCOPE)
set(ICU_ICUUC_LIBRARY ${LOCAL_ICU_LIB_DIR}/${ICUUC_SHARED_LIBRARY} PARENT_SCOPE)
set(ICU_LIBRARIES
    ${LOCAL_ICU_LIB_DIR}/${ICUDATA_SHARED_LIBRARY}
    ${LOCAL_ICU_LIB_DIR}/${ICUI18N_SHARED_LIBRARY}
    ${LOCAL_ICU_LIB_DIR}/${ICUUC_SHARED_LIBRARY}
    PARENT_SCOPE
)
set(ICU_PKGCONFIG_DIR ${LOCAL_ICU_LIB_DIR}/pkgconfig PARENT_SCOPE)

# (Copy and) deploy our copy of the ICU libraries

if(USE_PREBUILT_ICU_PACKAGE)
    set(DEPLOY_TARGET DIRECT)
else()
    set(DEPLOY_TARGET ${PACKAGE_BUILD})
endif()

foreach(ICU_LIBRARY ${ICUDATA_SHARED_LIBRARY} ${ICUI18N_SHARED_LIBRARY} ${ICUUC_SHARED_LIBRARY})
    linux_deploy_binary_file(${DEPLOY_TARGET} ${LOCAL_ICU_LIB_DIR} lib ${ICU_LIBRARY})
endforeach()
