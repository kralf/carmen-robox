remake_find_library(carmen-base_interface base_interface.h PACKAGE carmen)

remake_add_library(robox_base
  LINK ${ROBOX_LIBRARY} ${CARMEN_BASE_INTERFACE_LIBRARY})
remake_add_headers()
