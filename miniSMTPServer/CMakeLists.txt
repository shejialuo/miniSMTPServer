add_executable(miniSMTP miniSMTPServer.cpp)

set_target_properties(miniSMTP PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/)

target_include_directories(miniSMTP PRIVATE ./util)

target_link_libraries(miniSMTP util)

add_subdirectory(./util)
