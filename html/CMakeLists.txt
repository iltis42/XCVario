# Automatically embed (gzip compressed) assets into the project

# glob all *.html, *.css and *.js files for embedding
file(GLOB files "${project_dir}/html/*.html" "${project_dir}/html/*.css" "${project_dir}/html/*.js")

foreach(file ${files})
    if(CONFIG_WEBSERVER_USE_GZIP)

        message(STATUS "Add asset compressed: ${file}")

        # add command to generate .gz files for embedding
        add_custom_command(
            OUTPUT "${file}.gz"
            COMMAND python ${project_dir}/html/compress.py ${file}
        )

        # delete generated .gz file after building to force a update of the generated *.gz.S file
        add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME}.elf 
            POST_BUILD
            COMMAND cmake -E remove  "${file}.gz"
        )

        # Add file.ext.gz as BINARY file to ELF (like EMBED_FILES)
        target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "${file}.gz" BINARY)

    else()

        # add asset as plain TEXT file to binary
        target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "${file}" TEXT)

    endif()
endforeach()

