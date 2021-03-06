PROJECT(CITE_PARSER_SERVICE)

#SET(CMAKE_INSTALL_PREFIX $ENV{HOME}/.local/)
SET(VERSION 0.2)
SET(CMAKE_INCLUDE_DIRECTORIES_BEFORE on)
SET(CMAKE_INCLUDE_CURRENT_DIR on)
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX " ${CMAKE_INSTALL_PREFIX})

MESSAGE(STATUS "BINARY_DIR " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "SOURCE_DIR " ${PROJECT_SOURCE_DIR})

# debug
SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3 -Wall -g -ggdb -std=c99") # 
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wall -g -ggdb -std=c++11")
# release
#SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3 -std=c99")
#SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++11")



# REQUIRE
CMAKE_MINIMUM_REQUIRED(VERSION 1.0)


# INCLUDE DIRECTORIES
INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/include
    /usr/local/include
    /usr/include
)


# LINK DIRECTORIES
LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/lib
    ${PROJECT_BINARY_DIR}/lib64
    ${PROJECT_SOURCE_DIR}/.libs
    /usr/local/lib
    /usr/local/lib64
    /usr/lib
    /usr/lib64
)
#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/.libs)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(lib)

#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.ini ${PROJECT_BINARY_DIR}/config.ini )

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/token.ini ${PROJECT_BINARY_DIR}/token.ini)
FILE(COPY ${PROJECT_SOURCE_DIR}/db/dict.db DESTINATION ${PROJECT_BINARY_DIR}/db/ )
FILE(COPY ${PROJECT_SOURCE_DIR}/model/cite.model DESTINATION ${PROJECT_BINARY_DIR}/model/ )


#ADD_TEST(evolution ${EXECUTABLE_OUTPUT_PATH}/percimpl evo )
ENABLE_TESTING()


