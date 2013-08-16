Building the Software
=====================

Software requirements
---------------------

Several components are required to run vpn². Since this software is in its early
stages and has been tested on Fedora 19 (64 bit) only this list is considered
incomplete for the time being.

Compiletime requirements
^^^^^^^^^^^^^^^^^^^^^^^^

* gcc 4.7.1 with support for c++11.
* `Boost <http://www.boost.org/>`_ 1.54
	* `Boost ASIO <http://www.boost.org/doc/libs/release/libs/asio/>`_
	* `Boost Program Options <http://www.boost.org/doc/libs/release/libs/program_options/>`_
	* `Boost Serialization <http://www.boost.org/doc/libs/release/libs/serialization/>`_
* `GnuTLS <http://www.gnutls.org/>`_ 3.1.11
* `GnuPG Made Easy <http://www.gnupg.org/related_software/gpgme/>`_ (GPGME)
* `GNU IDN Library <http://www.gnu.org/software/libidn/>`_ (libidn2)
* `Sphinx <http://sphinx-doc.org/>`_ (to build manual)
* `Doxygen <http://www.stack.nl/~dimitri/doxygen/>`_ (to build api documentation)


Runtime requirements
^^^^^^^^^^^^^^^^^^^^

* `GnuPG <http://www.gnupg.org/>`_ 2.0.0 or later

