set(BOOST_COMPONENTS program_options signals thread serialization system regex log)
set(Boost_USE_MULTITHREADED ON)

set_property(DIRECTORY . PROPERTY EP_PREFIX .)

message(STATUS "To use the included boost library set USE_INTERNAL_BOOST")
option(USE_INTERNAL_BOOST "Use internal boost librabry" no)
set(USE_INTERNAL_BOOST ${USE_INTERNAL_BOOST} PARENT_SCOPE)
if(NOT USE_INTERNAL_BOOST)
	include(FindBoost)
	find_package(Boost 1.57.0 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
	set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} PARENT_SCOPE)
	set(Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE)
else()
	string(REPLACE ";" "," BOOST_COMPONENTS_LIST "${BOOST_COMPONENTS}")

	set(BOOST_DEBUG_FLAGS "")
	if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(BOOST_DEBUG_FLAGS "cxxflags=-D_GLIBCXX_CONCEPT_CHECK -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
	endif()

	set(BOOST_INCLUDED_VERSION TRUE PARENT_SCOPE)
	include(ExternalProject)
	ExternalProject_Add(libboost
		URL http://sourceforge.net/projects/boost/files/boost/1.57.0/boost_1_57_0.tar.bz2/download
		URL_HASH SHA512=61881440fd89644c43c6e3bc6292e9fed75a6d3a76f98654b189d0ed4e1087d77b585884e882270c08bf9f7132b173bfc1fde05848e06aa78ba7f1008d10714d
		CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap.sh --with-icu --prefix=<INSTALL_DIR> --with-libraries=${BOOST_COMPONENTS_LIST}
		BUILD_COMMAND <SOURCE_DIR>/b2 -a --build-dir=<BINARY_DIR> variant=release link=static threading=multi ${BOOST_DEBUG_FLAGS} stage
		INSTALL_COMMAND <SOURCE_DIR>/b2 -a --build-dir=<BINARY_DIR> variant=release link=static threading=multi ${BOOST_DEBUG_FLAGS} install
		BUILD_IN_SOURCE 1
	)

	foreach(BOOST_COMPONENT ${BOOST_COMPONENTS})
		set(BOOST_LINK_LIST "boost_${BOOST_COMPONENT}" ${BOOST_LINK_LIST})
	endforeach()
	set(Boost_LIBRARIES pthread ${BOOST_LINK_LIST} PARENT_SCOPE)
endif()

message(STATUS "To use the included GnuTLS library set USE_INTERNAL_GNUTLS")
option(USE_INTERNAL_GNUTLS "Use internal gnutls version" no)
set(USE_INTERNAL_GNUTLS ${USE_INTERNAL_GNUTLS} PARENT_SCOPE)
if(NOT USE_INTERNAL_GNUTLS)
	include(FindGnuTLS)
	find_package(GnuTLS 3.3.10 REQUIRED)
	set(GNUTLS_INCLUDE_DIR ${GNUTLS_INCLUDE_DIR} PARENT_SCOPE)
	set(GNUTLS_LIBRARIES ${GNUTLS_LIBRARIES} PARENT_SCOPE)
	set(GNUTLS_DEFINITIONS ${GNUTLS_DEFINITIONS} PARENT_SCOPE)
else()
	ExternalProject_Add(libgmp
		URL http://gmplib.org/download/gmp/gmp-5.1.3.tar.bz2
		URL_HASH SHA512=f42ce1fa02962d25f94e21c79837f0eac1f8fc8d5c7804b7413926be8739ae0e8da33c63b5cebad17b3b6caae495ace0ef2e54dd2f194a0870e85c48172382dc
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared=no CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0" "CXXFLAGS=-O3 -g0"
	)
	ExternalProject_Add(libnettle
		DEPENDS ${LIB_GMP_DEPEND}
		URL http://ftp.gnu.org/gnu/nettle/nettle-2.7.tar.gz
		URL_HASH SHA512=394cbe567f958594e1bca42b48802e2fbc9c31c468cd1c399a0aec6f94138220b4bf3c307590262314664e15a74a7a52f9fd2b4f9fb01dde669e1fbe23e57caf
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --libdir=<INSTALL_DIR>/lib --disable-shared --disable-openssl --disable-documentation LDFLAGS=-L<INSTALL_DIR>/lib CPPFLAGS=-I<INSTALL_DIR>/include CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0" "CXXFLAGS=-O3 -g0"
		BUILD_COMMAND make install
		INSTALL_COMMAND make install
	)
	ExternalProject_Add(libgnutls
		DEPENDS libnettle
		DOWNLOAD_COMMAND wget -qO- ftp://ftp.gnutls.org/gcrypt/gnutls/v3.3/gnutls-3.3.11.tar.xz | tar --strip-components=1 -JxC <SOURCE_DIR>
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared=no --without-p11-kit --without-tpm --disable-cxx CPPFLAGS=-I<INSTALL_DIR>/include CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O0 -g" "CXXFLAGS=-O0 -g" LDFLAGS=-L<INSTALL_DIR>/lib PKG_CONFIG_PATH=<INSTALL_DIR>/lib/pkgconfig
	)

	set(GnuTLS_LIBRARIES gnutls nettle hogweed rt z gmp tasn1 PARENT_SCOPE)
endif()
