#TER-Server

########### TER-Server-A ###############

file(GLOB_RECURSE sources_authentication Authentication/*.cpp Authentication/*.h)
file(GLOB_RECURSE sources_realms Realms/*.cpp Realms/*.h)
file(GLOB_RECURSE sources_server Server/*.cpp Server/*.h)
file(GLOB sources_localdir *.cpp *.h)

if (USE_COREPCH)
  set(TER-Server-A_PCH_HDR PrecompiledHeaders/authPCH.h)
  set(TER-Server-A_PCH_SRC PrecompiledHeaders/authPCH.cpp)
endif()

set(TER-Server-A_SRCS
  ${TER-Server-A_SRCS}
  ${sources_authentication}
  ${sources_realms}
  ${sources_server}
  ${sources_localdir}
)

if( WIN32 )
set(TER-Server-A_SRCS
    ${TER-Server-A_SRCS}
    ${sources_Debugging}
    TER-Server-A.rc
)
endif()

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography/Authentication
  ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
  ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Authentication
  ${CMAKE_CURRENT_SOURCE_DIR}/Realms
  ${CMAKE_CURRENT_SOURCE_DIR}/Server
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

add_executable(TER-Server-A
  ${TER-Server-A_SRCS}
  ${TER-Server-A_PCH_SRC}
)

add_dependencies(TER-Server-A revision.h)

if( NOT WIN32 )
  set_target_properties(TER-Server-A PROPERTIES
    COMPILE_DEFINITIONS _TRINITY_REALM_CONFIG="${CONF_DIR}/TER-Server-A.conf"
  )
endif()

target_link_libraries(TER-Server-A
  shared
  ${MYSQL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${ACE_LIBRARY}
)

if( WIN32 )
  add_custom_command(TARGET TER-Server-A
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/TER-Server-A.conf ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS TER-Server-A DESTINATION bin)
  install(FILES  TER-Server-A.conf DESTINATION etc)
elseif( WIN32 )
  install(TARGETS TER-Server-A DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES TER-Server-A.conf DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

# Generate precompiled header
if (USE_COREPCH)
  add_cxx_pch(TER-Server-A ${TER-Server-A_PCH_HDR} ${TER-Server-A_PCH_SRC})
endif()
