set(BOOST_COMPONENTS program_options signals thread serialization system regex)
set(Boost_USE_MULTITHREADED ON)

set_property(DIRECTORY . PROPERTY EP_PREFIX .)

message(STATUS "To use the included boost library set USE_INTERNAL_BOOST")
option(USE_INTERNAL_BOOST "Use internal boost librabry" no)
set(USE_INTERNAL_BOOST ${USE_INTERNAL_BOOST} PARENT_SCOPE)
if(NOT USE_INTERNAL_BOOST)
	include(FindBoost)
	find_package(Boost 1.55.0 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
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
		URL http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.bz2/download
		URL_HASH SHA512=dc606477f4c303e8f40de2586c16394b6d758e198b35bf3a7d3e576a2f49171aadc4f95e8d685fa731bc4e61e19869d5a24e1e816febfca9de078d66e096e041
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
	find_package(GnuTLS 3.2.12.1 REQUIRED)
	set(GNUTLS_INCLUDE_DIR ${GNUTLS_INCLUDE_DIR} PARENT_SCOPE)
	set(GNUTLS_LIBRARIES ${GNUTLS_LIBRARIES} PARENT_SCOPE)
	set(GNUTLS_DEFINITIONS ${GNUTLS_DEFINITIONS} PARENT_SCOPE)
else()
	ExternalProject_Add(libgmp
		URL https://gmplib.org/download/gmp/gmp-5.1.3.tar.bz2
		URL_HASH SHA512=f42ce1fa02962d25f94e21c79837f0eac1f8fc8d5c7804b7413926be8739ae0e8da33c63b5cebad17b3b6caae495ace0ef2e54dd2f194a0870e85c48172382dc
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared=no CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0" "CXXFLAGS=-O3 -g0"
	)
	ExternalProject_Add(libnettle
		DEPENDS ${LIB_GMP_DEPEND}
		URL http://www.lysator.liu.se/~nisse/archive/nettle-2.7.1.tar.gz
		URL_HASH SHA512=297c69e90bbd448f72e854abe5cc7868c08d710e1c1bcd6a14adf06e25629d58a3ef4d65ab588d001ec7091aa583032312ad15b416ea5479e5bf0ea63717f473
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --libdir=<INSTALL_DIR>/lib --disable-shared --disable-openssl --disable-documentation LDFLAGS=-L<INSTALL_DIR>/lib CPPFLAGS=-I<INSTALL_DIR>/include CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0" "CXXFLAGS=-O3 -g0"
		BUILD_COMMAND make install
		INSTALL_COMMAND make install
	)
	ExternalProject_Add(libgnutls
		DEPENDS libnettle
		DOWNLOAD_COMMAND wget -qO- ftp://ftp.gnutls.org/gcrypt/gnutls/v3.2/gnutls-3.2.13.tar.xz | tar --strip-components=1 -JxC <SOURCE_DIR>
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-shared=no --without-p11-kit --without-tpm --disable-cxx CPPFLAGS=-I<INSTALL_DIR>/include CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} "CFLAGS=-O3 -g0" "CXXFLAGS=-O3 -g0" LDFLAGS=-L<INSTALL_DIR>/lib PKG_CONFIG_PATH=<INSTALL_DIR>/lib/pkgconfig
	)

	set(GnuTLS_LIBRARIES gnutls nettle hogweed rt z gmp tasn1 PARENT_SCOPE)
endif()
