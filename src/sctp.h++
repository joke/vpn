#ifndef sctp_h
#define sctp_h

#include <boost/asio/basic_seq_packet_socket.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/basic_resolver.hpp>

class sctp {
public:
	//! The type of a SCTP endpoint.
	typedef boost::asio::ip::basic_endpoint<sctp> endpoint;

	//! The SCTP socket type.
	typedef boost::asio::basic_seq_packet_socket<sctp> socket;

	//! The SCTP acceptor type.
	typedef boost::asio::basic_socket_acceptor<sctp> acceptor;

	//! The SCTP resolver type.
	typedef boost::asio::ip::basic_resolver<sctp> resolver;

	//! Construct to represent the IPv4 SCTP protocol.
	static sctp v4() {
		return sctp(BOOST_ASIO_OS_DEF(AF_INET));
	}

	//! Construct to represent the IPv6 SCTP protocol.
	static sctp v6() {
		return sctp(BOOST_ASIO_OS_DEF(AF_INET6));
	}

	//! Obtain an identifier for the type of the protocol.
	int type() const {
		return SOCK_STREAM;
	}

	//! Obtain an identifier for the protocol.
	int protocol() const {
		return IPPROTO_SCTP;
	}

	//! Obtain an identifier for the protocol family.
	int family() const {
		return family_;
	}

	//! Compare two protocols for equality.
	friend bool operator==(const sctp& p1, const sctp& p2) {
		return p1.family_ == p2.family_;
	}

	//! Compare two protocols for inequality.
	friend bool operator!=(const sctp& p1, const sctp& p2) {
		return p1.family_ != p2.family_;
	}

private:
	// Construct with a specific family.
	explicit sctp(int protocol_family) : family_(protocol_family) {
	}

	int family_;
};

#endif
