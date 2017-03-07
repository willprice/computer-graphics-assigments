

set(command "/usr/bin/cmake;-Dmake=${make};-Dconfig=${config};-P;/home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/catch-stamp/catch-download-impl.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/catch-stamp/catch-download-out.log"
  ERROR_FILE "/home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/catch-stamp/catch-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/catch-stamp/catch-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "catch download command succeeded.  See also /home/ms14981/Documents/y3/graphics/template/ray-tracer/build_release/catch/src/catch-stamp/catch-download-*.log")
  message(STATUS "${msg}")
endif()
