#ifndef session_h
#define session_h

#include <boost/asio/socket_base.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/strand.hpp>
#include "encryption.h++"
#include <boost/system/system_error.hpp>

template <typename Protocol>
class session : protected gnutls::session {
public:
	typedef typename Protocol::socket socket;
	typedef typename socket::message_flags message_flags;

	explicit session(socket&& s, gnutls::credentials const& c, bool server) :
	gnutls::session(
		c,
		server,
		std::bind(&session::push, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&session::pull, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&session::pull_timeout, this, std::placeholders::_1),
		std::bind(&session::verify, this)
	),
		socket_(std::move(s)),
		strand_(socket_.get_io_service()),
		operation_((server) ? operation_state::start : operation_state::handshake),
		server_(server),
		unregister_(),
		ec_(EAGAIN, boost::system::system_category()),
		receive_out_flags_(),
		receive_in_flags_(),
		send_flags_(socket::message_end_of_record),
		bytes_available_(false)
	{
	}

	~session() {
	}

	void start() {
		do_handshake(boost::system::error_code(EAGAIN, boost::system::system_category()), -1);
	}

	void do_handshake(boost::system::error_code const& ec, std::size_t bytes_transferred) {
		bytes_available_ = true;
		auto res(handshake());
		if (res == 0)
			receive(&session::finish);
		else
			receive(&session::do_handshake);
	}

	void finish(boost::system::error_code const& ec, std::size_t bytes_transferred) {
// 		std::cerr << "_)))))))))))))))))))))))))))) finish ))))))))))))))))))" << std::endl;
	}

	void unregister(std::function<void()> f) {
		unregister_ = f;
	}

	void prefix(std::function<void(std::uint64_t const)> f) {
		prefix_ = f;
	}

	void forward(std::shared_ptr<std::vector<std::uint8_t>> v) {
		std::cerr << "___________________________________ SESSION::forward ________________________________________" << std::endl;
		send(v);
	}

protected:
	enum class operation_state {
		start,
		handshake,
		transfer
	};

	template <typename Handler>
	void receive(Handler&& h) {
		// 		std::cerr << "_________________________________receive(): available: " << socket_.available() << std::endl;
		socket_.async_receive(boost::asio::null_buffers(), receive_in_flags_, receive_out_flags_, std::bind(&session::do_handshake, this, std::placeholders::_1, std::placeholders::_2));
	}


	std::size_t pull(void* buffer, size_t buffer_size) {
		auto res = socket_.receive(boost::asio::buffer(buffer, buffer_size), receive_in_flags_, receive_out_flags_, ec_);
		bytes_available_ = false;
		return res;
	}

	std::size_t push(void const* buffer, size_t buffer_size) {
		return socket_.send(boost::asio::buffer(buffer, buffer_size), send_flags_);
	}

	int pull_timeout(unsigned int timeout) {
		// 		std::cerr << "_________________ pull_timeout: " << timeout << std::endl;

		if (bytes_available_)
			return 1;

		error(boost::system::error_code(EAGAIN, boost::system::system_category()));
		return -1;
	}

	int verify() {
		std::cerr << "______________________________________________ SESSION::verify ________________________________________" << std::endl;

		if (certificate_type() != GNUTLS_CRT_OPENPGP)
			return false;

		auto k = key();
		auto x = k.fingerprint();
		std::cerr << "______________________________________________ SESSION::verify size: " << x.size() << std::endl;
		std::cerr << to_hex(x.data(), x.data()+x.size()) << std::endl;

		x.data()[x.size()-16] = 0xfc;
		auto y = *reinterpret_cast<std::uint64_t*>(x.data()+x.size()-16);
		// 		auto z = __builtin_bswap64(*y);

		prefix_(y);

		return true;
	}

private:
	socket socket_;
	boost::asio::strand strand_;
	operation_state operation_;
	bool server_;


	std::function<void()> unregister_;
	std::function<void(std::uint64_t const)> prefix_;
	boost::system::error_code ec_;

	message_flags receive_out_flags_;
	message_flags receive_in_flags_;
	message_flags send_flags_;
	bool bytes_available_;
};

template <>
template <typename Handler>
void session<boost::asio::ip::tcp>::receive(Handler&& h) {
	socket_.async_receive(boost::asio::null_buffers(), receive_in_flags_, std::bind(h, this, std::placeholders::_1, std::placeholders::_2));
}

template <>
std::size_t session<boost::asio::ip::tcp>::pull(void* buffer, size_t buffer_size) {
// 	return socket_.receive(boost::asio::buffer(buffer_input_, buffer_input_size_), receive_in_flags_, std::bind(&session::do_operation, this, std::placeholders::_1, std::placeholders::_2));
}

#endif

