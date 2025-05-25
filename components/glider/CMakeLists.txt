
idf_component_register( SRCS "Polars.cpp"
                        INCLUDE_DIRS "include"
)

set(GENERATED_SRC "${CMAKE_CURRENT_BINARY_DIR}/PackedPolarTable.inc")
set(GENERATOR_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/polar_check.py")
set(INPUT_DATA_FILE "${CMAKE_CURRENT_SOURCE_DIR}/PolarTable.txt")

add_custom_command(
    OUTPUT ${GENERATED_SRC}
    COMMAND ${CMAKE_COMMAND} -E echo "Generating PackedPolarTable.inc"
    COMMAND python ${GENERATOR_SCRIPT}
            --input "${INPUT_DATA_FILE}"
            --output "${GENERATED_SRC}"
    DEPENDS ${GENERATOR_SCRIPT}
    COMMENT "Generating packed glider polars"
    VERBATIM
)

# Ensure the component depends on the generated file
target_sources(${COMPONENT_LIB} PRIVATE ${GENERATED_SRC})
target_include_directories(${COMPONENT_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

