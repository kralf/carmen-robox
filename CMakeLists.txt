cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  carmen-robox
  VERSION 0.1
  RELEASE alpha
  SUMMARY "CARMEN modules for the RoboX platform."
  AUTHOR "Ralf Kaestner"
  CONTACT "ralf.kaestner@gmail.com"
  HOME "http://github.com/kralf/carmen-robox"
  LICENSE "GNU Lesser General Public License (LGPL)"
  INSTALL /usr
  PREFIX carmen-
)

remake_doc(
  html
)

remake_pack_deb(
  DEPENDS librobox carmen-interface carmen-era
)
remake_pack_deb(
  COMPONENT dev
  DESCRIPTION "development headers"
  DEPENDS carmen-robox librobox-dev carmen-interface-dev
)
remake_pack_deb(
  ARCH all
  COMPONENT doc
  DESCRIPTION "documentation"
)

remake_distribute_deb(
  DISTRIBUTION lucid
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS carmen-dev carmen-era-dev librobox-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE CARMEN_ROBOX_GIT_REVISION
)
remake_distribute_deb(
  DISTRIBUTION precise
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS carmen-dev carmen-era-dev librobox-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE CARMEN_ROBOX_GIT_REVISION
)
remake_distribute_deb(
  DISTRIBUTION trusty
  SECTION libs
  UPLOAD ppa:kralf/asl
  DEPENDS carmen-dev carmen-era-dev librobox-dev remake pkg-config doxygen
  PASS CMAKE_BUILD_TYPE CARMEN_ROBOX_GIT_REVISION
)
