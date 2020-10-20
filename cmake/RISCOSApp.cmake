# Use the following commands to build for RISC OS:
# cmake -B build-riscos -DCMAKE_BUILD_TYPE=Release -DBACKEND_PLATFORM=SDL1 -DBACKEND_RENDERER=Software -DBACKEND_AUDIO=SDL1 -DDOCONFIG=OFF -DCMAKE_TOOLCHAIN_FILE=$GCCSDK_INSTALL_ENV/toolchain-riscos.cmake -DRISCOS=ON -DPKG_CONFIG_STATIC_LIBS=ON
# cmake --build build-riscos
# (cd game_english && $GCCSDK_INSTALL_ENV/bin/zip -,9r cse2-riscos.zip \!CSE2)

function(elf_to_aif)
	cmake_parse_arguments(ELFTOAIF "" "TARGET;OUTPUT" "" ${ARGN})
	get_filename_component(ELFTOAIF_OUTPUT_DIR "${ELFTOAIF_OUTPUT}" DIRECTORY)
	add_custom_command(OUTPUT "${ELFTOAIF_OUTPUT}"
	                   COMMAND ${CMAKE_COMMAND} -E make_directory ${ELFTOAIF_OUTPUT_DIR}
	                   COMMAND elf2aif $<TARGET_FILE:${ELFTOAIF_TARGET}> ${ELFTOAIF_OUTPUT}
	                   DEPENDS ${ELFTOAIF_TARGET})
	add_custom_target(${ELFTOAIF_TARGET}-aif ALL DEPENDS ${ELFTOAIF_OUTPUT})
endfunction(elf_to_aif)

elf_to_aif(TARGET CSE2 OUTPUT ${BUILD_DIRECTORY}/!CSE2/CSE2,ff8)

configure_file(${ASSETS_DIRECTORY}/riscos/!Boot,feb ${BUILD_DIRECTORY}/!CSE2/!Boot,feb COPYONLY)
configure_file(${ASSETS_DIRECTORY}/riscos/!Run,feb ${BUILD_DIRECTORY}/!CSE2/!Run,feb COPYONLY)
configure_file(${ASSETS_DIRECTORY}/riscos/!Sprites,ff9 ${BUILD_DIRECTORY}/!CSE2/!Sprites,ff9 COPYONLY)
configure_file(${BUILD_DIRECTORY}/licence.txt ${BUILD_DIRECTORY}/!CSE2/Licence COPYONLY)
file(COPY ${BUILD_DIRECTORY}/data DESTINATION ${BUILD_DIRECTORY}/!CSE2)
