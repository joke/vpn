set(BOOST_COMPONENTS program_options signals thread serialization system regex)
set(Boost_USE_MULTITHREADED ON)

set_property(DIRECTORY . PROPERTY EP_PREFIX .)

message(STATUS "To use the included boost library set USE_INTERNAL_BOOST")
option(USE_INTERNAL_BOOST "Use internal boost librabry" no)
set(USE_INTERNAL_BOOST ${USE_INTERNAL_BOOST} PARENT_SCOPE)
if(NOT USE_INTERNAL_BOOST)
	include(FindBoost)
	find_package(Boost 1.53.0 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
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
		URL http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.bz2/download
		URL_MD5 15cb8c0803064faef0c4ddf5bc5ca279
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
	find_package(GnuTLS 3.2.4 REQUIRED)
	set(GNUTLS_INCLUDE_DIR ${GNUTLS_INCLUDE_DIR} PARENT_SCOPE)
	set(GNUTLS_LIBRARIES ${GNUTLS_LIBRARIES} PARENT_SCOPE)
	set(GNUTLS_DEFINITIONS ${GNUTLS_DEFINITIONS} PARENT_SCOPE)
else()
	ExternalProject_Add(libgmp
		URL ftp://ftp.gmplib.org/pub/gmp-5.1.2/gmp-5.1.2.tar.bz2
		URL_MD5 7e3516128487956cd825fef01aafe4bc
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared=no CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0"
	)
	ExternalProject_Add(libnettle
		DEPENDS ${LIB_GMP_DEPEND}
		URL http://www.lysator.liu.se/~nisse/archive/nettle-2.7.tar.gz
		URL_MD5 2caa1bd667c35db71becb93c5d89737f
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --libdir=<INSTALL_DIR>/lib --disable-shared LDFLAGS=-L<INSTALL_DIR>/lib CPPFLAGS=-I<INSTALL_DIR>/include CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0"
		BUILD_COMMAND make install
		INSTALL_COMMAND make install
	)
	ExternalProject_Add(libgnutls
		DEPENDS libnettle
		DOWNLOAD_COMMAND wget -qO- ftp://ftp.gnutls.org/gcrypt/gnutls/v3.2/gnutls-3.2.4.tar.xz | tar --strip-components=1 -JxC <SOURCE_DIR>
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared=no --without-p11-kit --without-tpm --disable-cxx CPPFLAGS=-I<INSTALL_DIR>/include CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0" LDFLAGS=-L<INSTALL_DIR>/lib PKG_CONFIG_PATH=<INSTALL_DIR>/lib/pkgconfig
	)

	set(GnuTLS_LIBRARIES gnutls nettle hogweed rt z gmp tasn1 PARENT_SCOPE)
endif()
