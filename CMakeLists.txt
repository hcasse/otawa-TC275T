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
add_definitions(-g3 -O2)

# configuration
set(SCRIPT		"tricore16")		# machine model
set(NAMESPACE	"otawa")	# namespace (classes must be contained in the namespace otawa::model)
set(FILES		pipeline16E.xml pipeline16P.xml mem.xml cache16P.xml cache16E.xml)				# cache, pipeline, memory
set(MODULES	)				# used modules (to pass to otawa-config=
#set(SOURCES 	clp_analysis.cpp clp_symbolic_expr.cpp ContextualValueInfo.cpp MemoryAccessIdentifier.cpp AddressExtracter.cpp CLPSimulator.cpp CLPInitiator.cpp verification.cpp tricore16P.cpp prod16P.h mreg.h pipe.h)				# sources of the plugin
set(SOURCES 	ContextualValueInfo.cpp MemoryAccessIdentifier.cpp AddressExtracter.cpp CLPSimulator.cpp CLPInitiator.cpp AbstractStateVerifier.cpp PrefetchCategoryAnalysis.cpp BBTimerTC16P.cpp BBTimerTC16E.cpp TC16.h prodTC16.cpp mregTC16.cpp pipeTC16.cpp CacheBlockFinder.cpp)				# sources of the plugin


# C++ 11
if(CMAKE_VERSION LESS "3.1")
	add_compile_options(--std=c++11)
	message(STATUS "C++11 set using cflags")
else()
	set(CMAKE_CXX_STANDARD 11)
	message(STATUS "C++ set using CMAKE_CXX_STANDARD")
endif()


# generation of prods
set(TRICORE_IRG	"${CMAKE_SOURCE_DIR}/../tricore/tricore.irg")
set(GLISS_ATTR	"${CMAKE_SOURCE_DIR}/../gliss2/gep/gliss-attr")
add_custom_command(
	OUTPUT		"prod.h"
	DEPENDS 	"prod.tpl" "TC16.nmp" ${TRICORE_IRG}
	COMMAND 	${GLISS_ATTR}
	ARGS 		${TRICORE_IRG} -o prod.h -a prod -f -t "prod.tpl" -d "return 2;" -e ${CMAKE_SOURCE_DIR}/TC16.nmp
	VERBATIM 
)
add_custom_command(
	OUTPUT		"mreg.h"
	DEPENDS 	"mreg.tpl" "TC16.nmp" ${TRICORE_IRG}
	COMMAND 	${GLISS_ATTR}
	ARGS 		${TRICORE_IRG} -o mreg.h -a mreg -f -t "mreg.tpl" -d "return 0;" -e ${CMAKE_SOURCE_DIR}/TC16.nmp
	VERBATIM 
)
add_custom_command(
	OUTPUT		"pipe.h"
	DEPENDS 	"pipe.tpl" "TC16.nmp" ${TRICORE_IRG}
	COMMAND 	${GLISS_ATTR}
	ARGS 		${TRICORE_IRG} -o pipe.h -a pipe -f -t "pipe.tpl" -d "return 0;" -e ${CMAKE_SOURCE_DIR}/TC16.nmp
	VERBATIM 
)
#add_custom_command(
#	OUTPUT		"deps.h"
#	DEPENDS 	"deps.tpl" "deps.nmp" ${TRICORE_IRG}
#	COMMAND 	${GLISS_ATTR}
#	ARGS 		${TRICORE_IRG} -o "deps.h" -a deps -p -t "deps.tpl" -d ";" -e "${CMAKE_SOURCE_DIR}/deps.nmp"
#	VERBATIM 
#)

add_custom_command(
	OUTPUT		"prodTC16.cpp"
	DEPENDS 	"prod.h"
	COMMAND 	cp
	ARGS 		prod.h prodTC16.cpp
	VERBATIM 
)

add_custom_command(
	OUTPUT		"pipeTC16.cpp"
	DEPENDS 	"pipe.h"
	COMMAND 	cp
	ARGS 		pipe.h pipeTC16.cpp
	VERBATIM 
)

add_custom_command(
	OUTPUT		"mregTC16.cpp"
	DEPENDS 	"mreg.h"
	COMMAND 	cp
	ARGS 		mreg.h mregTC16.cpp
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


set(TARGET_LIB	"${CMAKE_SOURCE_DIR}/../tricore/src/libtricore.a")

# plugin definition
set(CMAKE_INSTALL_RPATH "\\$ORIGIN/../../../")
include_directories("${CMAKE_SOURCE_DIR}" "../tricore/include")
add_library(${SCRIPT} SHARED ${SOURCES})
set_property(TARGET ${SCRIPT} PROPERTY PREFIX "")
set_property(TARGET ${SCRIPT} PROPERTY COMPILE_FLAGS "${OTAWA_CFLAGS}")
target_link_libraries(${SCRIPT} "${OTAWA_LDFLAGS}" "${OTAWA_PREFIX}/lib/otawa/otawa/etime${CMAKE_SHARED_LIBRARY_SUFFIX}"  "${TARGET_LIB}")

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
# install(FILES ${SCRIPT}.osx DESTINATION ${SCRIPT_PATH})
install(FILES tricore16E.osx DESTINATION ${SCRIPT_PATH})
install(FILES tricore16P.osx DESTINATION ${SCRIPT_PATH})
foreach(FILE ${FILES})
	install(FILES ${SCRIPT}/${FILE} DESTINATION ${SCRIPT_PATH}/${SCRIPT})
endforeach()

