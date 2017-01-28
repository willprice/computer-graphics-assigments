include(ExternalProject)
find_package(Git REQUIRED)

if(IS_TRAVIS_BUILD)     # on travis, use git for fetching instead of wget
    set(FETCH_EXTERNAL_CATCH
        GIT_REPOSITORY
        https://github.com/philsquared/Catch.git
        GIT_TAG
        9a566095699756b409ff17baca04311c586360d1)
else()
    set(FETCH_EXTERNAL_CATCH
        URL
        https://github.com/philsquared/Catch/archive/v1.6.1.zip
        URL_HASH
        MD5=1600a9cd833a76ebe74a2761fffaa9b3)
endif()

ExternalProject_Add(catch
    PREFIX ${CMAKE_BINARY_DIR}/catch
    DOWNLOAD_COMMAND ${FETCH_EXTERNAL_CATCH}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

ExternalProject_Get_Property(catch source_dir)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${source_dir}/single_include)
