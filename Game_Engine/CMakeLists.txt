project(Game_Engine)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADERS
 ${HEADER_DIR}/TextureCache.h
 ${HEADER_DIR}/RandomNumber.h
 ${HEADER_DIR}/IGameObject.h
 ${HEADER_DIR}/IShape.h
 ${HEADER_DIR}/VectorND.h
 ${HEADER_DIR}/Animation.h
)
set(SOURCES
${SOURCE_DIR}/TextureCache.cpp
${SOURCE_DIR}/RandomNumber.cpp
${SOURCE_DIR}/IGameObject.cpp
${SOURCE_DIR}/IShape.cpp
${SOURCE_DIR}/Animation.cpp
)

add_library(${PROJECT_NAME}

${SOURCES}
${HEADERS}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${HEADER_DIR}>
)

target_include_directories(Game_Engine PUBLIC ${SFML_INCLUDE_DIR}) 
link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Game
        Scene
    PRIVATE
        sfml-graphics-d
        sfml-window-d
        sfml-system-d
        sfml-audio-d
        sfml-network-d
)
add_custom_command(TARGET Game_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SFML_BIN_DIR} $<TARGET_FILE_DIR:Game_Engine>
)


set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Game_Engine")