SET(EMSCRIPTEN_PATH $ENV{HOME}/perso/emscripten)
SET(EMSCRIPTEN_BUILD 1)
SET(CMAKE_C_COMPILER ${EMSCRIPTEN_PATH}/emcc)
SET(CMAKE_CXX_COMPILER ${EMSCRIPTEN_PATH}/emcc)
SET(CMAKE_C_FLAGS "-O0 -g -DDEBUG -DEMSCRIPTEN")
SET(CMAKE_CXX_FLAGS "-O0 -g -DDEBUG -DEMSCRIPTEN -DUSE_VBO")
