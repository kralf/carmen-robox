remake_find_library(carmen-era_interface era_interface.h PACKAGE "CARMEN ERA"
  OPTIONAL)

if(CARMEN_ERA_INTERFACE_FOUND)
  remake_define(WITH_ERA 1)
  remake_add_executables(LINK robox_base
    ${CARMEN_ERA_INTERFACE_LIBRARY} ${CARMEN_PARAM_INTERFACE_LIBRARY})
else(CARMEN_ERA_INTERFACE_FOUND)
  remake_add_executables(LINK robox_base ${CARMEN_PARAM_INTERFACE_LIBRARY})
endif(CARMEN_ERA_INTERFACE_FOUND)
