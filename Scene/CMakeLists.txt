project(Scene)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(HEADERS
 ${HEADER_DIR}/ScoreManager.h
 ${HEADER_DIR}/SceneManager.h
 ${HEADER_DIR}/SceneBase.h
 ${HEADER_DIR}/Button.h
 ${HEADER_DIR}/MenuScenes.h
)
set(SOURCES
${SOURCE_DIR}/ScoreManager.cpp
${SOURCE_DIR}/SceneManager.cpp
${SOURCE_DIR}/SceneBase.cpp
${SOURCE_DIR}/Button.cpp
${SOURCE_DIR}/MenuScenes.cpp
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

target_include_directories(Scene PUBLIC ${SFML_INCLUDE_DIR}) 
link_directories(${SFML_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Game
        Game_Engine
    PRIVATE
        sfml-graphics-d
        sfml-window-d
        sfml-system-d
        sfml-audio-d
        sfml-network-d
)
add_custom_command(TARGET Scene POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SFML_BIN_DIR} $<TARGET_FILE_DIR:Scene>
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Scene")