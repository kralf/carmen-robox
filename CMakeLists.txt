cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  "CARMEN RoboX"
  VERSION 0.1
  RELEASE alpha
  SUMMARY "CARMEN modules for the RoboX platform."
  AUTHOR "Ralf Kaestner"
  CONTACT "ralf.kaestner@gmail.com"
  HOME "http://aslforge.ethz.ch/projects/carmen-robox"
  LICENSE "GNU Lesser General Public License (LGPL)"
  INSTALL /usr
  PREFIX carmen-
)

remake_pack_deb(carmen-interface tulibs)
remake_pack_deb(
  COMPONENT dev
  DESCRIPTION "development headers"
  carmen_robox carmen-interface-dev tulibs-dev
)
remake_pack_deb(
  ARCH all
  COMPONENT doc
  DESCRIPTION "documentation"
)
