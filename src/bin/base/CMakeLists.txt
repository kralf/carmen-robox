if(CARMEN_ERA_FOUND)
  remake_define(WITH_ERA 1)
  remake_add_executable(carmen-robox robox.c PREFIX OFF
    LINK robox_base ${CARMEN_ERA_LIBRARIES} ${CARMEN_INTERFACE_LIBRARIES})
else(CARMEN_ERA_FOUND)
  remake_add_executable(carmen-robox robox.c PREFIX OFF
    LINK robox_base ${CARMEN_INTERFACE_LIBRARIES})
endif(CARMEN_ERA_FOUND)
