FILE(GLOB Sources "*.h" "*.cpp")
ADD_LIBRARY(sparkplug-gl ${Sources})

TARGET_LINK_LIBRARIES(sparkplug-gl sparkplug-core)


FIND_PACKAGE(OpenGL REQUIRED)
	INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(sparkplug-gl ${OPENGL_gl_LIBRARY})

FIND_PACKAGE(GLEW REQUIRED)
	INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(sparkplug-gl ${GLEW_LIBRARIES})


FILE(GLOB PublicHeaders RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
SET_TARGET_PROPERTIES(sparkplug-gl PROPERTIES PUBLIC_HEADER "${PublicHeaders}")
SET_TARGET_PROPERTIES(sparkplug-gl PROPERTIES VERSION "${VERSION}")

INSTALL(TARGETS sparkplug-gl
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION "include/SparkPlug/GL"
)

if(UNIX)
    SET(PKG_DEPS "sparkplug-core >= ${VERSION}, gl, glew")
    SET(PKG_LIBS "")
    SET(LIB_NAME "sparkplug-gl.${VERSION}")

    CONFIGURE_FILE(
        ${CMAKE_SOURCE_DIR}/sparkplug-gl.pc.in
        ${CMAKE_BINARY_DIR}/sparkplug-gl.pc @ONLY
    )

    INSTALL(
        FILES ${CMAKE_BINARY_DIR}/sparkplug-gl.pc
        DESTINATION lib${LIB_SUFFIX}/pkgconfig
    )
endif()
