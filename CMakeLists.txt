cmake_minimum_required(VERSION 3.10)

project(FOOTBALL
  VERSION 0.0
  LANGUAGES CXX
)

# Configurer le fichier config.h
set(FOOTBALL_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/code/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

find_package(gf REQUIRED)

# Ajouter l'exécutable
add_executable(football
  code/football.cc
  code/ball.cc
  code/player.cc
  code/team.cc
  code/field.cc
  code/minimap.cc
)

# Ajouter les répertoires pour inclure les headers
target_include_directories(football
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Lier la bibliothèque
target_link_libraries(
  football
    gf::graphics
    gf::net
)

# Installer le binaire
install(
  TARGETS football
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

# Installer les données
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/games"
)