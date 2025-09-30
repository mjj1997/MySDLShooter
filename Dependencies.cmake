include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(MySDLShooter_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  find_package(SDL2 CONFIG REQUIRED)
  find_package(SDL2_image CONFIG REQUIRED)
  find_package(SDL2_mixer CONFIG REQUIRED)
  find_package(SDL2_ttf CONFIG REQUIRED)

endfunction()
