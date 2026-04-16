set(project_name install_monitor_srv)
project ("${project_name}")

aux_source_directory(. SOURCES)
add_executable(${project_name} ${SOURCES})
target_link_libraries(${project_name} PRIVATE Version Pathcch)
