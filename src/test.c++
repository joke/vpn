#include <iostream>
#include <boost/program_options.hpp>

#include <boost/asio.hpp>
#include <vector>



class dccp {
public:
	/// The type of a DCCP endpoint.
	typedef boost::asio::ip::basic_endpoint<dccp> endpoint;

	/// Construct to represent the IPv4 DCCP protocol.
	static dccp v4() {
		return dccp(BOOST_ASIO_OS_DEF(AF_INET));
	}

	/// Construct to represent the IPv6 DCCP protocol.
	static dccp v6() {
		return dccp(BOOST_ASIO_OS_DEF(AF_INET6));
	}

	/// Obtain an identifier for the type of the protocol.
	int type() const {
		return SOCK_DCCP;
	}

	/// Obtain an identifier for the protocol.
	int protocol() const {
		return IPPROTO_DCCP;
	}

	/// Obtain an identifier for the protocol family.
	int family() const {
		return family_;
	}

	/// The DCCP socket type.
	typedef boost::asio::basic_seq_packet_socket<dccp> socket;

	/// The DCCP acceptor type.
	typedef boost::asio::basic_socket_acceptor<dccp> acceptor;

	/// The DCCP resolver type.
	typedef boost::asio::ip::basic_resolver<dccp> resolver;

	/// Compare two protocols for equality.
	friend bool operator==(const dccp& p1, const dccp& p2) {
		return p1.family_ == p2.family_;
	}

	/// Compare two protocols for inequality.
	friend bool operator!=(const dccp& p1, const dccp& p2) {
		return p1.family_ != p2.family_;
	}

private:
	// Construct with a specific family.
	explicit dccp(int protocol_family) : family_(protocol_family) {
	}

	int family_;
};

int main() {

	boost::asio::io_service io;
	using namespace boost::asio;


	dccp::endpoint endp(ip::address::from_string("::"), 54321);

	dccp::acceptor acceptor(io, endp);
	dccp::socket socket(io);
	dccp::endpoint endpoint;
	acceptor.accept(socket, endpoint);

	std::cerr << endpoint << std::endl;


	io.run();
}
