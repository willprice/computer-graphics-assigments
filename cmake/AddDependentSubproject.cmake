# Source :
# https://github.com/Barthelemy/CppProjectTemplate/blob/master/cmake/Common.cmake

function(add_dependent_subproject subproject_name)
    # When a project has already been defined, then
    # PROJECT_${subproject_name} will be defined
    if(NOT PROJECT_${subproject_name})
        find_package(${subproject_name} CONFIG REQUIRED)
    else()
        include_directories(../${subproject_name}/include)
    endif()
endfunction(add_dependent_subproject)
