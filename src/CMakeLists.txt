include_directories(/usr/include/libxml2/)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wconversion -pedantic -ansi")
SET(LINK_FLAGS "-g")

SET(SRC_FILES area.cpp common.cpp entity.cpp log.cpp main.cpp resourcer.cpp
  sprite.cpp window.cpp world.cpp)

ADD_EXECUTABLE(tsunagari ${SRC_FILES})
TARGET_LINK_LIBRARIES(tsunagari gosu zip xml2)
INSTALL(TARGETS tsunagari DESTINATION .)

