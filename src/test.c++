#include <iostream>
#include <boost/program_options.hpp>

#include <boost/asio.hpp>
#include <vector>

namespace services {

/// Debugging stream socket service that wraps the normal stream socket service.
template <typename Protocol>
class datagram_socket_service : public boost::asio::io_service::service {
private:
    /// The type of the wrapped stream socket service.
    typedef boost::asio::datagram_socket_service<Protocol> service_impl_type;

public:
    /// The unique service identifier.
    static boost::asio::io_service::id id;

    /// The protocol type.
    typedef Protocol protocol_type;

    /// The endpoint type.
    typedef typename Protocol::endpoint endpoint_type;

    /// The implementation type of a stream socket.
    typedef typename service_impl_type::implementation_type implementation_type;

    /// The native type of a stream socket.
    typedef typename service_impl_type::native_handle_type native_handle_type;

    /// Construct a new stream socket service for the specified io_service.
    explicit datagram_socket_service(boost::asio::io_service& io_service) : boost::asio::io_service::service(io_service), service_impl_(boost::asio::use_service<service_impl_type>(io_service)) {
    }

    /// Destroy all user-defined handler objects owned by the service.
    void shutdown_service() {
    }

    /// Construct a new stream socket implementation.
    void construct(implementation_type& impl) {
        service_impl_.construct(impl);
    }

    /// Destroy a stream socket implementation.
    void destroy(implementation_type& impl) {
        service_impl_.destroy(impl);
    }

    /// Open a new stream socket implementation.
    boost::system::error_code open(implementation_type& impl, const protocol_type& protocol, boost::system::error_code& ec) {
        return service_impl_.open(impl, protocol, ec);
    }

    /// Open a stream socket from an existing native socket.
    boost::system::error_code assign(implementation_type& impl, const protocol_type& protocol, const native_handle_type& native_socket, boost::system::error_code& ec) {
        return service_impl_.assign(impl, protocol, native_socket, ec);
    }

    /// Determine whether the socket is open.
    bool is_open(const implementation_type& impl) const {
        return service_impl_.is_open(impl);
    }

    /// Close a stream socket implementation.
    boost::system::error_code close(implementation_type& impl, boost::system::error_code& ec) {
        return service_impl_.close(impl, ec);
    }

    /// Determine whether the socket is at the out-of-band data mark.
    bool at_mark(const implementation_type& impl, boost::system::error_code& ec) const {
        return service_impl_.at_mark(impl, ec);
    }

    /// Determine the number of bytes available for reading.
    std::size_t available(const implementation_type& impl, boost::system::error_code& ec) const {
        return service_impl_.available(impl, ec);
    }

    /// Bind the stream socket to the specified local endpoint.
    boost::system::error_code bind(implementation_type& impl, const endpoint_type& endpoint, boost::system::error_code& ec) {
        return service_impl_.bind(impl, endpoint, ec);
    }

    /// Connect the stream socket to the specified endpoint.
    boost::system::error_code connect(implementation_type& impl, const endpoint_type& peer_endpoint, boost::system::error_code& ec) {
        return service_impl_.connect(impl, peer_endpoint, ec);
    }

    /// Start an asynchronous connect.
    template <typename Handler>
    void async_connect(implementation_type& impl, const endpoint_type& peer_endpoint, Handler handler) {
        //service_impl_.async_connect(impl, peer_endpoint, connect_handler<Handler>(handler, logger_));
    }

    /// Set a socket option.
    template <typename Option>
    boost::system::error_code set_option(implementation_type& impl, const Option& option, boost::system::error_code& ec) {
        return service_impl_.set_option(impl, option, ec);
    }

    /// Get a socket option.
    template <typename Option>
    boost::system::error_code get_option(const implementation_type& impl, Option& option, boost::system::error_code& ec) const {
        return service_impl_.get_option(impl, option, ec);
    }

    /// Perform an IO control command on the socket.
    template <typename IO_Control_Command>
    boost::system::error_code io_control(implementation_type& impl, IO_Control_Command& command, boost::system::error_code& ec) {
        return service_impl_.io_control(impl, command, ec);
    }

    /// Get the local endpoint.
    endpoint_type local_endpoint(const implementation_type& impl, boost::system::error_code& ec) const {
        return service_impl_.local_endpoint(impl, ec);
    }

    /// Get the remote endpoint.
    endpoint_type remote_endpoint(const implementation_type& impl, boost::system::error_code& ec) const {
        return service_impl_.remote_endpoint(impl, ec);
    }

    /// Disable sends or receives on the socket.
    boost::system::error_code shutdown(implementation_type& impl, boost::asio::socket_base::shutdown_type what, boost::system::error_code& ec) {
        return service_impl_.shutdown(impl, what, ec);
    }

	/// Start an asynchronous receive.
	template <typename Mutable_Buffers, typename Handler>
	void async_receive_from(implementation_type& impl, const Mutable_Buffers& buffers, endpoint_type& e, boost::asio::socket_base::message_flags flags, Handler handler) {
		service_impl_.async_receive(impl, buffers, flags, [&handler](const boost::system::error_code& e, std::size_t bytes_transferred) {
			handler(e, bytes_transferred);
		});
	}

private:
    /// The wrapped stream socket service.
    service_impl_type& service_impl_;

    /// The logger used for writing debug messages.
//     mutable logger logger_;
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
		std::cerr << "___________________________________________" << std::endl;
	});



	io.run();
}
