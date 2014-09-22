#ifndef session_h
#define session_h

#include "encryption.h++"
#include <boost/asio/ip/udp.hpp>
#include <map>
#include <set>
#include <mutex>
#include <iostream>


namespace std {

template <>
class hash<boost::asio::ip::basic_endpoint<boost::asio::ip::udp>> {
public:
	std::size_t operator()(boost::asio::ip::basic_endpoint<boost::asio::ip::udp> const& endpoint) const {
		return reinterpret_cast<std::size_t>(endpoint.data());
	}
};

} // namespace: std


template <typename Protocol>
class session : private gnutls::session {
public:
	using endpoint_type = typename Protocol::endpoint;

	explicit session() = delete;
	explicit session(session const&) = delete;
	session(session&& s) : gnutls::session(std::forward<gnutls::session>(s)), buffer_(nullptr, 0) {
		::gnutls_transport_set_ptr(session_, this);
	}

	explicit session(bool server, gnutls::credentials const& c, gnutls::priorities const& p) : gnutls::session(server, c, p), buffer_(nullptr, 0) {
		::gnutls_transport_set_ptr(session_, this);
		::gnutls_transport_set_pull_function(session_, &session::pull_buffer);
		::gnutls_transport_set_push_function(session_, &session::push_buffer);
		::gnutls_heartbeat_enable(session_, GNUTLS_HB_PEER_ALLOWED_TO_SEND);
		::gnutls_dtls_set_mtu(session_, 1500);
	}

	void decrypt(boost::asio::mutable_buffers_1 b) {
		buffer_ = std::move(b);
		auto res = ::gnutls_handshake(session_);
		std::cerr << "x2x_________________________________________________________________________" << res << std::endl;
	}


	~session() {}

	boost::asio::mutable_buffers_1 buffer_;




//! send data. bufffer with unencrypted data
void send(boost::asio::const_buffers_1);
//! receive data. buffer with encrypted  and buffer where to place data
//void receive(boost::asio::const_buffers_1, boost::asio::mutable_buffers_1);
void receive() {}

private:
	static long int pull_buffer(gnutls_transport_ptr_t p, void* d, std::size_t s) {
		std::cerr << ":: " << __func__ << " ::" << std::endl;

		return boost::asio::buffer_copy(boost::asio::buffer(d, s), static_cast<session*>(p)->buffer_);
	}

	static long int push_buffer(gnutls_transport_ptr_t p, void const* d, std::size_t s) {
		std::cerr << ":: " << __func__ << " ::" << std::endl;

	}

};


template <typename Session>
class session_builder {
public:
	using session_type = session<Session>;

	explicit session_builder() = delete;

	explicit session_builder(boost::asio::io_service const& io, gnutls::credentials const& c, gnutls::priorities const& p) : io_(io), credentials_(c), priorities_(p) {
	}

	session_builder(session_builder const& sb) : io_(sb.io_), credentials_(sb.credentials_) {
	}

	session_type construct(bool server) const {
		session_type session(server, credentials_, priorities_);

		return std::move(session);
	}

protected:
	boost::asio::io_service const& io_;
	gnutls::credentials const& credentials_;
	gnutls::priorities const& priorities_;
};

#ifndef IN_IDE_PARSERER
#include "session.t++"
#endif


#endif
