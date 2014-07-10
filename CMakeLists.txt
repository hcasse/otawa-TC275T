# To use this template, complete the configuration section.
# This file must be saved with name "CMakeLists.txt".
#
# The created plugin will be named ${SCRIPT}.so and the contained classes
# must be put in the C++ namespace ${NAMESPACE}::${SCRIPT}. The installed
# OSX file must be called ${SCRIPT}.osx and put in the same directory
# than this file.
#
# Script files like pipeline, cache and memory description must be set
# in the FILES variable.
#
# Sources must be set in the SOURCES variable.
#
# To compile it, otawa-config must in the path:
#	cmake .
# Or passed explicetely:
#	cmake . -DOTAWA_CONFIG=path/to/otawa-config

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# configuration
set(SCRIPT		"tricore16P")		# machine model
set(NAMESPACE	"continental")	# namespace (classes must be contained in the namespace otawa::model)
set(FILES		pipeline16P.xml mem.xml cache16P.xml)				# cache, pipeline, memory
set(MODULES	)				# used modules (to pass to otawa-config=
set(SOURCES 	tricore16P.cpp prod16P.h)				# sources of the plugin


# generation of prods
set(TRICORE_IRG	"${CMAKE_SOURCE_DIR}/../tricore/tricore.irg")
set(GLISS_ATTR	"${CMAKE_SOURCE_DIR}/../gliss2/gep/gliss-attr")
add_custom_command(
	OUTPUT		"prod16P.h"
	DEPENDS 	"prod.tpl" "prod16P.nmp" ${PPC_IRG}
	COMMAND 	${GLISS_ATTR}
	ARGS 		${TRICORE_IRG} -o prod16P.h -a prod -f -t "prod.tpl" -d "return 2;" -e ${CMAKE_SOURCE_DIR}/prod16P.nmp
	VERBATIM 
)


# script
project(${SCRIPT})
include_directories("${CMAKE_SOURCE_DIR}/include")


# look for OTAWA
if(NOT OTAWA_CONFIG)
	find_program(OTAWA_CONFIG otawa-config DOC "path to otawa-config")
	if(NOT OTAWA_CONFIG)
		message(FATAL_ERROR "ERROR: otawa-config is required !")
	endif()
endif()
message(STATUS "otawa-config at ${OTAWA_CONFIG}")
execute_process(COMMAND "${OTAWA_CONFIG}" --cflags ${MODULES} OUTPUT_VARIABLE OTAWA_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${OTAWA_CONFIG}" --libs ${MODULES}  OUTPUT_VARIABLE OTAWA_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND "${OTAWA_CONFIG}" --prefix OUTPUT_VARIABLE OTAWA_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)


# plugin definition
set(CMAKE_INSTALL_RPATH "\\$ORIGIN/../../../")
include_directories("${CMAKE_SOURCE_DIR}" "../tricore/include")
add_library(${SCRIPT} SHARED ${SOURCES})
set_property(TARGET ${SCRIPT} PROPERTY PREFIX "")
set_property(TARGET ${SCRIPT} PROPERTY COMPILE_FLAGS "${OTAWA_CFLAGS}")
target_link_libraries(${SCRIPT} "${OTAWA_LDFLAGS}" "${OTAWA_PREFIX}/lib/otawa/proc/otawa/etime${CMAKE_SHARED_LIBRARY_SUFFIX}")

# installation
if(NOT PREFIX)
	set(PREFIX "${OTAWA_PREFIX}")
endif()
set(MODULE_PATH "${PREFIX}/lib/otawa/proc/${NAMESPACE}")
set(SCRIPT_PATH "${PREFIX}/share/Otawa/scripts/")

if(WIN32 OR WIN64)
	install(TARGETS ${SCRIPT} RUNTIME DESTINATION ${MODULE_PATH})
else()
	install(TARGETS ${SCRIPT} LIBRARY DESTINATION ${MODULE_PATH})
endif()
install(FILES ${SCRIPT}.eld DESTINATION ${MODULE_PATH})
install(FILES ${SCRIPT}.osx DESTINATION ${SCRIPT_PATH})
foreach(FILE ${FILES})
	install(FILES ${SCRIPT}/${FILE} DESTINATION ${SCRIPT_PATH}/${SCRIPT})
endforeach()

