# src/

# include settings
#INCLUDE_DIRECTORIES(${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/lib)
#INCLUDE_DIRECTORIES(${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/lib/include)
INCLUDE_DIRECTORIES(${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/include)

# lib settings
#LINK_DIRECTORIES(${${CMAKE_PROJECT_NAME}_BINARY_DIR}/lib)
LINK_DIRECTORIES(${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/.libs)



# all related files
AUX_SOURCE_DIRECTORY(. DIR_TARGET)
ADD_LIBRARY (relfiles ${DIR_TARGET})

################################################################################

#ADD_EXECUTABLE(ngd ng.cc)

#  executable dependencies
#ADD_DEPENDENCIES(ngd argcvnw)

#  executable target links
#TARGET_LINK_LIBRARIES(ngd relfiles)
#TARGET_LINK_LIBRARIES(ngd argcvnw_s)

#  executable properties
#SET_TARGET_PROPERTIES(ngd PROPERTIES OUTPUT_NAME "ngd")


#ADD_EXECUTABLE(ng_cli ng_cli.cc)

#  executable dependencies
#ADD_DEPENDENCIES(ng_cli argcvnw)

#  executable target links
#TARGET_LINK_LIBRARIES(ng_cli relfiles)
#TARGET_LINK_LIBRARIES(ng_cli argcvnw_s)

#  executable properties
#SET_TARGET_PROPERTIES(ng_cli PROPERTIES OUTPUT_NAME "ng_cli")

ADD_EXECUTABLE(citepd citepd.cc)

#  executable dependencies
ADD_DEPENDENCIES(citepd relfiles)
ADD_DEPENDENCIES(citepd cite_parser_s)
ADD_DEPENDENCIES(citepd cjson_s)
ADD_DEPENDENCIES(citepd argcvnw)

#  executable target links
TARGET_LINK_LIBRARIES(citepd relfiles)
TARGET_LINK_LIBRARIES(citepd cite_parser_s)
TARGET_LINK_LIBRARIES(citepd cjson_s)
TARGET_LINK_LIBRARIES(citepd argcvnw_s)
#TARGET_LINK_LIBRARIES(citepd dl)
#TARGET_LINK_LIBRARIES(citepd pthread)
#TARGET_LINK_LIBRARIES(citepd m)
#TARGET_LINK_LIBRARIES(citepd crfpp)

#  executable properties
SET_TARGET_PROPERTIES(citepd PROPERTIES OUTPUT_NAME "citepd")


ADD_EXECUTABLE(cpcli cpcli.cc)

#  executable dependencies
ADD_DEPENDENCIES(cpcli relfiles)
ADD_DEPENDENCIES(cpcli cite_parser_s)
ADD_DEPENDENCIES(cpcli cjson_s)
ADD_DEPENDENCIES(cpcli argcvnw)

#  executable target links
TARGET_LINK_LIBRARIES(cpcli relfiles)
TARGET_LINK_LIBRARIES(cpcli cite_parser_s)
TARGET_LINK_LIBRARIES(cpcli cjson_s)
TARGET_LINK_LIBRARIES(cpcli argcvnw_s)
#TARGET_LINK_LIBRARIES(cpcli dl)
#TARGET_LINK_LIBRARIES(cpcli pthread)
#TARGET_LINK_LIBRARIES(cpcli m)
#TARGET_LINK_LIBRARIES(cpcli crfpp)

#  executable properties
SET_TARGET_PROPERTIES(cpcli PROPERTIES OUTPUT_NAME "cpcli")





