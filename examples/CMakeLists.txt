set(FLT2VRML_SRCS
  flt2vrml/array.c
  flt2vrml/bsptree.c
  flt2vrml/flt2vrml.c
)
set(FLT2VRML_HDRS
  flt2vrml/array.h
  flt2vrml/bsptree.h
)
add_executable(flt2vrml ${FLT2VRML_SRCS} ${FLT2VRML_HDRS})
target_link_libraries(flt2vrml PRIVATE profit)
if(NOT WIN32)
  target_link_libraries(flt2vrml PRIVATE m)
endif()

set(FLTDUMP_SRCS fltdump/fltdump.c)
add_executable(fltdump ${FLTDUMP_SRCS})
target_link_libraries(fltdump PRIVATE profit)

set(FLTNODE_SRCS fltnode/fltnode.c)
add_executable(fltnode ${FLTNODE_SRCS})
target_link_libraries(fltnode PRIVATE profit)

set(FLTSTRIP_SRCS fltstrip/fltstrip.c)
add_executable(fltstrip ${FLTSTRIP_SRCS})
target_link_libraries(fltstrip PRIVATE profit)

install(TARGETS flt2vrml fltdump fltnode fltstrip EXPORT ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime NAMELINK_SKIP
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
)
