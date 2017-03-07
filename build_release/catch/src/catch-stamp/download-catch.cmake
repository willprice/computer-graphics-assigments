if(EXISTS "/home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/v1.6.1.zip")
  file("MD5" "/home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/v1.6.1.zip" hash_value)
  if("x${hash_value}" STREQUAL "x1600a9cd833a76ebe74a2761fffaa9b3")
    return()
  endif()
endif()
message(STATUS "downloading...
     src='https://github.com/philsquared/Catch/archive/v1.6.1.zip'
     dst='/home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/v1.6.1.zip'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/philsquared/Catch/archive/v1.6.1.zip"
  "/home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/v1.6.1.zip"
  SHOW_PROGRESS
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/philsquared/Catch/archive/v1.6.1.zip' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
