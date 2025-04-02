
project(Game)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADERS
 ${HEADER_DIR}/Game.h
 ${HEADER_DIR}/GameObject.h
 ${HEADER_DIR}/Hero.h
 ${HEADER_DIR}/Collision.h
 ${HEADER_DIR}/ISpawner.h
 ${HEADER_DIR}/MegaBoss.h
 ${HEADER_DIR}/BossBullet.h
 ${HEADER_DIR}/Enemy.h
)
set(SOURCES
${SOURCE_DIR}/Game.cpp
${SOURCE_DIR}/GameObject.cpp
${SOURCE_DIR}/Hero.cpp
${SOURCE_DIR}/Collision.cpp
${SOURCE_DIR}/ISpawner.cpp
${SOURCE_DIR}/MegaBoss.cpp
${SOURCE_DIR}/BossBullet.cpp
${SOURCE_DIR}/Enemy.cpp
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
target_include_directories(Game PUBLIC ${SFML_INCLUDE_DIR}) 

link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Game_Engine
    Scene
    PRIVATE
        sfml-graphics-d
        sfml-window-d
        sfml-system-d
        sfml-audio-d
        sfml-network-d
        
)
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SFML_BIN_DIR} $<TARGET_FILE_DIR:Game>
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Game")
