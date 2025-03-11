project(Game_Engine)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADERS
 ${HEADER_DIR}/TextureCache.h
 ${HEADER_DIR}/SceneManager.h
 ${HEADER_DIR}/SceneBase.h
 ${HEADER_DIR}/RandomNumber.h
 ${HEADER_DIR}/IGameObject.h
 ${HEADER_DIR}/IShape.h
)
set(SOURCES
${SOURCE_DIR}/TextureCache.cpp
${SOURCE_DIR}/SceneManager.cpp
${SOURCE_DIR}/SceneBase.cpp
${SOURCE_DIR}/RandomNumber.cpp
${SOURCE_DIR}/IGameObject.cpp
${SOURCE_DIR}/IShape.cpp
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
include_directories(${CMAKE_SOURCE_DIR}/KT_Library/KT_Library/include)
target_include_directories(Game_Engine PUBLIC ${SFML_INCLUDE_DIR}) 
link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Game
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