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
		gnutls::session(c, server, this, &base_push, &base_pull, &base_pull_timeout),
		socket_(std::move(s)),
		strand_(socket_.get_io_service()),
		operation_((server) ? operation_state::start : operation_state::handshake),
		server_(server),
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

// 		std::cerr << "_____________________________________do_handshake() ec: " << ec << " bytes_transferred: " << bytes_transferred << std::endl;
		auto res = handshake();
// 		std::cerr << "_____________________________________do_handshake() res: " << res << std::endl;
		if (res == 0)
			receive(&session::finish);
		else
			receive(&session::do_handshake);
	}

	void finish(boost::system::error_code const& ec, std::size_t bytes_transferred) {
// 		std::cerr << "_)))))))))))))))))))))))))))) finish ))))))))))))))))))" << std::endl;
	}

	template <typename Handler>
	void receive(Handler&& h) {
// 		std::cerr << "_________________________________receive(): available: " << socket_.available() << std::endl;
		socket_.async_receive(boost::asio::null_buffers(), receive_in_flags_, receive_out_flags_, std::bind(&session::do_handshake, this, std::placeholders::_1, std::placeholders::_2));
	}



	std::size_t pull(void* buffer, size_t buffer_size) {
// 		std::cerr << "____________________________________pull()  buffer : " << buffer << " buffer_size: " << buffer_size << std::endl;
		auto res = socket_.receive(boost::asio::buffer(buffer, buffer_size), receive_in_flags_, receive_out_flags_, ec_);
		bytes_available_ = false;
// 		std::cerr << "____________________________________pull()  buffer : " << buffer << " buffer_size: " << buffer_size << " res: " << res << " ec: " << ec_ << std::endl;
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

	void unregister(std::function<void()> f) {
		unregister_ = f;
	}

protected:
	static ssize_t base_push(void* s, void const* buffer, size_t buffer_size) {
		return static_cast<session*>(s)->push(buffer, buffer_size);
	}

	static ssize_t base_pull(void* s, void* buffer, size_t buffer_size) {
		return static_cast<session*>(s)->pull(buffer, buffer_size);
	}

	static int base_pull_timeout(void* s, unsigned int timeout) {
		return static_cast<session*>(s)->pull_timeout(timeout);
	}

	enum class operation_state {
		start,
		handshake,
		transfer
	};


private:
	socket socket_;
	boost::asio::strand strand_;

	bool handshake_completed_;
	bool server_;
	bool bytes_available_;

	operation_state operation_;

	std::function<void()> unregister_;
	boost::system::error_code ec_;

	message_flags receive_in_flags_;
	message_flags receive_out_flags_;
	message_flags send_flags_;
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

