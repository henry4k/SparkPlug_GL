# - Try to find GLFW3
# Once done this will define
#
#  GLFW3_FOUND - system has GLFW
#  GLFW3_INCLUDE_DIR - the include directory
#  GLFW3_LIBRARY - The GLFW library


FIND_PATH( GLFW3_INCLUDE_DIR GL/glfw3.h $ENV{GLFWDIR}/include
                                      ~/Library/Frameworks/GLFW3.framework/Headers
                                      /Library/Frameworks/GLFW3.framework.Headers
                                      /usr/local/include
                                      /usr/local/X11R6/include
                                      /usr/X11R6/include
                                      /usr/X11/include
                                      /usr/include/X11
                                      /usr/include
                                      /opt/X11/include
                                      /opt/include )

FIND_LIBRARY( GLFW3_LIBRARY NAMES glfw3 PATHS $ENV{GLFWDIR}/lib
                                              $ENV{GLFWDIR}/support/msvc80/Debug
                                              $ENV{GLFWDIR}/support/msvc80/Release
                                              /usr/local/lib
                                              /usr/local/X11R6/lib
                                              /usr/X11R6/lib
                                              /usr/X11/lib
                                              /usr/lib/X11
                                              /usr/lib
                                              /opt/X11/lib
                                              /opt/lib )

MARK_AS_ADVANCED(GLFW3_INCLUDE_DIR GLFW3_LIBRARY)

IF(GLFW3_INCLUDE_DIR AND GLFW3_LIBRARY)
	SET(GLFW3_FOUND TRUE)
ELSE(GLFW3_INCLUDE_DIR AND GLFW3_LIBRARY)
	SET(GLFW3_FOUND FALSE)
ENDIF(GLFW3_INCLUDE_DIR AND GLFW3_LIBRARY)
