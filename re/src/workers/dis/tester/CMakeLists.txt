set(PROJ_NAME dis_worker_tester)
set(TX_NAME dis_tx)
set(RX_NAME dis_rx)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

#RX tester utility
add_executable(${RX_NAME} "")
# Compile againt cxx_std_11 to avoid dynamic exception deprecation
target_compile_features(${RX_NAME} PRIVATE cxx_std_11)
# Suppress dynamic exception and auto_ptr deprecation warnings
target_compile_options(${RX_NAME} PRIVATE -Wno-deprecated -Wno-deprecated-delarations)
target_sources(${RX_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rx.cpp)
target_link_libraries(${RX_NAME} PRIVATE ${KDIS_LIB})
target_include_directories(${RX_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../kdis)

# TX tester utility
add_executable(${TX_NAME} "")
# Compile againt cxx_std_11 to avoid dynamic exception deprecation
target_compile_features(${TX_NAME} PRIVATE cxx_std_11)
# Suppress dynamic exception and auto_ptr deprecation warnings
target_compile_options(${TX_NAME} PRIVATE -Wno-deprecated -Wno-deprecated-delarations)
target_sources(${TX_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tx.cpp)
target_link_libraries(${TX_NAME} PRIVATE ${KDIS_LIB})
target_include_directories(${TX_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../kdis)

# Tester
add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
# Suppress dynamic exception and auto_ptr deprecation warnings
target_compile_options(${PROJ_NAME} PRIVATE -Wno-deprecated -Wno-deprecated-delarations)
target_sources(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJ_NAME} PUBLIC dis_worker)
