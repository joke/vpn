if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-std=c++1y")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -g0")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -pg -pedantic -DBOOST_ASIO_ENABLE_BUFFER_DEBUGGING -DBOOST_ASIO_ENABLE_HANDLER_TRACKING -D_XOPEN_SOURCE -D_GLIBCXX_CONCEPT_CHECK")# -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")# -Wextra -Wall -Wunused")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif()

link_directories(${CMAKE_BINARY_DIR}/lib/lib)
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/lib/include)
add_definitions(${GNUTLS_DEFINITIONS})


find_library(GPGME_LIBRARIES gpgme-pthread)
if (NOT GPGME_LIBRARIES)
	message(FATAL_ERROR "GPGME library required")
endif()
find_library(IDN2_LIBRARIES idn2)
if (NOT IDN2_LIBRARIES)
	message(FATAL_ERROR "IDN2 library required")
endif()


add_executable(vpn main.c++ program_options.c++ encryption.c++)
target_link_libraries(vpn ${GnuTLS_LIBRARIES} ${Boost_LIBRARIES} ${GPGME_LIBRARIES} ${IDN2_LIBRARIES})

add_executable(experimental test.c++)
target_link_libraries(experimental ${GnuTLS_LIBRARIES} ${Boost_LIBRARIES} ${GPGME_LIBRARIES} ${IDN2_LIBRARIES})

install(TARGETS vpn DESTINATION "${CMAKE_INSTALL_PREFIX}/sbin")

if(USE_INTERNAL_BOOST)
	add_dependencies(vpn libboost)
endif()
if(USE_INTERNAL_GNUTLS)
	add_dependencies(vpn libgnutls)
endif()
