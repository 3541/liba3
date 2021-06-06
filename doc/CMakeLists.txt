option(BUILD_DOC "Build documentation" ON)

if (BUILD_DOC)
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOC_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOXYFILE})
    add_custom_command(
      OUTPUT ${DOC_OUTPUT}
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../README.md ${DOXYFILE} a3 ${A3_HEADERS}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating docs."
      VERBATIM
    )

    add_custom_target(a3_doc DEPENDS ${DOC_OUTPUT})
  else()
    message(WARN "BUILD_DOC is enabled but Doxygen could not be found.")
  endif()
endif()
