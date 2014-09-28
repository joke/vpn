#include <iostream>
#include <boost/program_options.hpp>

#include <boost/asio.hpp>
#include <vector>

namespace services {

template <typename Protocol>
class datagram_socket_service : public boost::asio::datagram_socket_service<Protocol> {
private:
	typedef boost::asio::datagram_socket_service<Protocol> base_type;

public:
	/// The unique service identifier.
	static boost::asio::io_service::id id;

	typedef Protocol protocol_type;
	typedef typename Protocol::endpoint endpoint_type;
	typedef typename base_type::implementation_type implementation_type;
	typedef typename base_type::native_handle_type native_handle_type;

	explicit datagram_socket_service(boost::asio::io_service& io_service) : base_type(io_service) {
	}

	void shutdown_service() {
		std::cerr << "_________ shutdown_service _________________" << std::endl;
	}

    /// Construct a new stream socket implementation.
    void construct(implementation_type& impl) {
		std::cerr << "_________ construct _________________" << std::endl;
		int a = impl;
		base_type::construct(impl);
    }
//
//     /// Destroy a stream socket implementation.
//     void destroy(implementation_type& impl) {
// 		std::cerr << "_________ destruct _________________" << std::endl;
// 		base_type::destroy(impl);
//     }
//

	/// Start an asynchronous receive.
	template <typename Mutable_Buffers, typename Handler>
	void async_receive_from(implementation_type& impl, const Mutable_Buffers& buffers, endpoint_type& e, boost::asio::socket_base::message_flags flags, Handler handler) {
		base_type::async_receive(impl, buffers, flags, [&handler](const boost::system::error_code& e, std::size_t bytes_transferred) {
			handler(e, bytes_transferred);
		});
	}

};

template <typename Protocol>
boost::asio::io_service::id datagram_socket_service<Protocol>::id;

}






typedef boost::asio::basic_datagram_socket<boost::asio::ip::udp, services::datagram_socket_service<boost::asio::ip::udp>> debug_datagram_socket;


int main() {


	boost::asio::io_service io;

	debug_datagram_socket::endpoint_type endp(boost::asio::ip::address::from_string("::"), 54321);
	debug_datagram_socket socket(io, endp);
	std::vector<char> buf(1024);

	socket.async_receive_from(boost::asio::buffer(buf), endp, [](const boost::system::error_code& error, std::size_t bytes_transferred) {
		std::cerr << "1___________________________________________" << bytes_transferred << std::endl;
	});
	socket.async_receive_from(boost::asio::buffer(buf), endp, [](const boost::system::error_code& error, std::size_t bytes_transferred) {
		std::cerr << "2___________________________________________" << bytes_transferred << std::endl;
	});



	boost::asio::ip::udp::endpoint unp(boost::asio::ip::address::from_string("::"), 54320);
	boost::asio::ip::udp::socket sock(io, unp);

	sock.async_receive_from(boost::asio::buffer(buf), unp, [](const boost::system::error_code& error, std::size_t bytes_transferred) {
		std::cerr << "3___________________________________________" << bytes_transferred << std::endl;
	});

	io.run();


}
