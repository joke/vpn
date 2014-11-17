#ifndef session_h
#define session_h

#include <boost/asio/socket_base.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include "encryption.h++"

template <typename Protocol>
class session {
public:
	typedef typename Protocol::socket socket;
	typedef typename socket::message_flags message_flags;

	explicit session(socket&& s) : socket_(std::move(s)), buffer_() {
		setup_receive();
	}

	void receive(boost::system::error_code const& error, std::size_t bytes_transferred) {
		std::cerr << "error: " << error << std::endl;
		std::cerr << "bytes_transferred: " << bytes_transferred << std::endl;
		std::cerr << "in: " << in_flags_ << std::endl;
		std::cerr << "out: " << out_flags_ << std::endl;

		if (error || !bytes_transferred)
			unregister_();
		else
			setup_receive();
	}

	~session() {
	}

	void unregister(std::function<void()> f) {
		unregister_ = f;
	}

private:
	void setup_receive() {
		using namespace std;
		socket_.async_receive(boost::asio::buffer(buffer_), in_flags_, out_flags_, bind(&session::receive, this, placeholders::_1, placeholders::_2));
	}

	std::function<void()> unregister_;
	std::array<char, 1280> buffer_;
	socket socket_;
	message_flags in_flags_;
	message_flags out_flags_;
};

template <>
void session<boost::asio::ip::tcp>::setup_receive() {
	using namespace std;
	socket_.async_receive(boost::asio::buffer(buffer_), out_flags_, bind(&session::receive, this, placeholders::_1, placeholders::_2));
}


#endif
