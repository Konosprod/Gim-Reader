# set project's name
  PROJECT( gim-reader )
  
  cmake_minimum_required(VERSION 2.8)
  
  # with SET() command you can change variables or define new ones
  # here we define THINK_SRCS variable that contains a list of all .cpp files
  # note that we don't need \ at the end of line
  SET( GIM_READER_SRCS
    main.c
    gim.c
  )
  set(EXECUTABLE_OUTPUT_PATH ${GIM_READER_SOURCE_DIR})
  
  # and finally an resource file
  
  # enable warnings
  ADD_DEFINITIONS( -Wall -W -ansi -pedantic -std=c99)
  
  FIND_PACKAGE( SDL REQUIRED )
  
  if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
  endif ( NOT SDL_FOUND )


  INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
  
  # here we instruct CMake to build "THINK" executable from all of the source files
  ADD_EXECUTABLE( gim-reader ${GIM_READER_SRCS})
  
  # last thing we have to do is to tell CMake what libraries our executable needs,
  # luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
  TARGET_LINK_LIBRARIES( gim-reader ${SDL_LIBRARY} SDLmain)
  
  install(
    TARGETS
        gim-reader
    RUNTIME DESTINATION
        ${CMAKE_INSTALL_PREFIX}/bin
)

