remake_find_package(carmen-interface CONFIG)
remake_find_package(carmen CONFIG)
remake_find_package(librobox CONFIG)
remake_find_package(carmen-era CONFIG OPTIONAL)

remake_define(REDHAT_52 ON)
remake_define(REDHAT_6 ON)
remake_define(REDHAT_71 ON)

remake_add_directories()
