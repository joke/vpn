Architecture of the vpn
=======================

vpn² is a virtual private network software much like the popular `OpenVPN
<http://www.openvpn.net/>`_. But unlike OpenVPN is based on a peer-to-peer
architecture to alleviate the need for central server.

Features
--------

* Decentral organisation of the vpn
	* No dedicated server is required


Technical Overview
------------------

.. index::
	peer-to-peer, p2p, DTLS, TLS, GnuPGP, OpenPGP, Web-of-Trust, IPv6, unique
	local address, mDNS, Routing

* vpn² peers use direct (peer-to-peer) connections to communicate between each
  other

	* Peers operate as client and server altogether
	* No single point of failure

* Direct encrypted connection between peers:

	* UDP transport with TLS encryption using DTLS (:rfc:`6347`)
	* TLS heartbeat extension (:rfc:`6520`) to
	
		* Prevent NAT timeout
		* Monitor state of peers

* Authentication based on OpenPGP (:rfc:`6091`)

	* Using local GnuPG keyring
	* Trust delegation using trusted signatures
	* OpenPGP’s Web-of-Trust algorithm
	* Key exchange during TLS handshake possible

* IPv6 (:rfc:`4291`) addresses used for addressing vpn² peers

	* Address prefix ``fc00::/8`` used for vpn²
	* Auto address assignment of unique local address (ULA, :rfc:`4193`) based
	  on OpenPGP certificate hash and ``fc00::/64`` prefix
	* LAN connectivity
	
		* Routing of prefix ``fc00::/64`` between LAN and vpn
		* Stateless address autoconfiguration (SLAAC, :rfc:`4862`) of prefix for
		  LAN access
		* Routing of multicast addresses (:rfc:`4291#section-2.7`) with site-local scope
		  or link-local scope to support mDNS (:rfc:`6762`)

* Decentral organization

	* No configuration needed
	* Addresses not assigned by central authority
	* Low chance of address conflicts due to IPv6 address space

* Implementation in Userspace

	* C++11 code base
	* GnuTLS encryption
	* Boost.ASIO network library
	* Userspace software using tun/tap kernel support
