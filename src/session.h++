#ifndef session_h
#define session_h

#include <boost/asio/socket_base.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
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
		buffer_(),
		bytes_transferred_(-1),
		ec_(EAGAIN, boost::system::system_category()),
		out_flags_(socket::message_end_of_record),
		buffer_output_(nullptr),
		buffer_input_(nullptr),
		buffer_output_size_(0),
		buffer_input_size_(0),
		determine_buffer_(true)
	{
	}

	~session() {
		std::cerr << "_____________________ ~SESSION() ____________________________" << std::endl;
	}

	void start() {
		shake_hands(boost::system::error_code(0, boost::system::system_category()), -1);
	}

	void unregister(std::function<void()> f) {
		unregister_ = f;
	}

	void shake_hands(boost::system::error_code const& ec, std::size_t bytes_transferred) {
		ec_ = ec;

		buffer_input_size_ = bytes_transferred;
		if (handshake() == -28)
			receive();
	}

	void receive() {
		socket_.async_receive(boost::asio::buffer(buffer_input_, buffer_input_size_), in_flags_, out_flags_, std::bind(&session::shake_hands, this, std::placeholders::_1, std::placeholders::_2));
	}


	ssize_t pull(void* buffer, size_t buffer_size) {
		std::cerr << "_______________________________________ PULL() " << buffer << " determine_buffer_: " << determine_buffer_ << std::endl;

		if (determine_buffer_) {
			buffer_input_ = buffer;
			buffer_input_size_ = buffer_size;
			error(boost::system::error_code(EAGAIN, boost::system::system_category()));
			determine_buffer_ = false;
			return -1;
		} else {
			determine_buffer_ = true;
			return buffer_input_size_;
		}
	}

	ssize_t push(void const* buffer, size_t buffer_size) {
		std::cerr << "_______________________PUSH()::::::::::: " << buffer << " size: " << buffer_size << std::endl;

		return socket_.send(boost::asio::buffer(buffer, buffer_size), out_flags_);
	}

	int pull_timeout(unsigned int timeout) {
		std::cerr << "______________________________ pull_timeout(): timeout: " << timeout << " buffer_input_size_: " << buffer_input_size_ << std::endl;
		return 1;
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

private:
	std::function<void()> unregister_;
	std::array<char, 1280> buffer_;
	boost::system::error_code ec_;
	std::size_t bytes_transferred_;
	socket socket_;
	message_flags in_flags_;
	message_flags out_flags_;

	bool determine_buffer_;

	void const* buffer_output_;
	std::size_t buffer_output_size_;
	void* buffer_input_;
	std::size_t buffer_input_size_;
};

template <>
void session<boost::asio::ip::tcp>::receive() {
	socket_.async_receive(boost::asio::buffer(buffer_input_, buffer_input_size_), out_flags_, std::bind(&session::shake_hands, this, std::placeholders::_1, std::placeholders::_2));
}

#endif
