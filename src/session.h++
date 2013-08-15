#ifndef session_h
#define session_h

#include "encryption.h++"
#include <boost/asio/ip/udp.hpp>
#include <map>
#include <set>
#include <mutex>

namespace {

template <typename Gateway>
class session {
public:
	typedef Gateway gateway_type;
	typedef typename Gateway::protocol_type protocol_type;
	typedef typename Gateway::endpoint_type endpoint_type;
	typedef typename Gateway::prefix_type prefix_type;
	typedef typename Gateway::endpoint_handle_type endpoint_handle_type;
	typedef typename Gateway::prefix_handle_type prefix_handle_type;
	typedef typename std::map<boost::asio::ip::udp::endpoint, session*>::iterator endpoint_iterator_type;
	typedef typename std::map<std::uint64_t, session*>::iterator prefix_iterator_type;

	//! create client session with a set of endpoints to try to connect with
	explicit session(boost::asio::io_service&, Gateway& g, std::set<endpoint_type> const&);
	//! create a server session with a specific endpoint
	explicit session(boost::asio::io_service&, Gateway& g, endpoint_type const&);
	//! close session the proper way. bye
	~session();

	//! set endpoint handler. called by session if a endpoint is connected
	void set(endpoint_handle_type const&);
	//! set prefix handler. called by session if a prefix has been calculated
	void set(prefix_handle_type const&);
	//! set cookie. called by gateway
	void set(gnutls::cookie&);

	//! send data. bufffer with unencrypted data
	void send(boost::asio::const_buffers_1);
	//! receive data. buffer with encrypted  and buffer where to place data
	void receive(boost::asio::const_buffers_1, boost::asio::mutable_buffers_1);
	//! get endpoint iterator. used to un-register
	void endpoint(endpoint_iterator_type it);
	//! if someone needs the handle
	endpoint_iterator_type endpoint() const;
	//! get prefix iterator. used to un-register
	void prefix(prefix_iterator_type it);
	//! if someone needs the handle
	prefix_iterator_type prefix() const;
	//! verify function called by gnutls
	static int verify(gnutls_session_t);

protected:
	//! private constructor to delegate
	explicit session(boost::asio::io_service&, Gateway&, endpoint_type const&, std::set<endpoint_type> const&, bool server);
	//! get gnutls session
	operator gnutls_session_t();

	//! send tls-alert
	void alert(bool fatal, gnutls_alert_description_t);
	//! switch to next endpoint if current endpoint is dead
	void next_endpoint();
	//! send goodbye
	void bye();
	//! ping send alert
	void ping(boost::system::error_code const&);
	//! respond to incoming pong
	void pong();
	//! terminate session by bye() and ~session()
	void terminate(boost::system::error_code const& = boost::system::error_code());
	//! do a single handshake
	void handshake();
	//! if the handshake timer expired call handshake again or terminate
	void handshake_timer_expired(boost::system::error_code const& = boost::system::error_code());
	//! set asynchronous timers call register &handshake
	void set_handshake_timer();
	//! gnutls transport timeout function
	static int timeout(gnutls_transport_ptr_t, unsigned int);
	//! gnutls pull function
	static std::ptrdiff_t pull(gnutls_transport_ptr_t, void* buffer, std::size_t);
	//! gnutls push function
	static std::ptrdiff_t push(gnutls_transport_ptr_t p, const void* buffer, std::size_t);

	boost::asio::io_service& io_; //!< proactor
	gateway_type& gateway_; //!< gateway we a associated with
	boost::asio::high_resolution_timer timer_; //!< timer for handshake and ping
	gnutls_session_t session_; //!< gnutls session
	std::mutex session_mutex_; //!< mutex used to lock session
	endpoint_type endpoint_; //!< current endpoint
	std::set<typename Gateway::endpoint_type> endpoints_; //!< list of endpoints
	endpoint_handle_type endpoint_handle_; //!< handle to un-register at gateway
	prefix_handle_type prefix_handle_; //!< handle to un-register at gateway
	boost::asio::const_buffers_1 receive_src_; //!< have we received bytes yet
	endpoint_iterator_type endpoint_iterator_;
	prefix_iterator_type prefix_iterator_;
	bool server_; //!< server session?
	bool verified_; //!< is the session verified?
	bool terminate_; //!< is the session going to be terminated?
	bool handshake_completed_; //!< handshake complete?
	std::size_t bytes_available_; //!< are bytes available?
	std::size_t pings_missed_; //!< number of pings missed before termination
	prefix_type prefix_; //!< prefix set during verification

private:
	explicit session() = delete;
	explicit session(session const&) = delete;
	explicit session(session const&&) = delete;
	session& operator=(session const&) = delete;
	session& operator=(session const&&) = delete;
};

} // namespace: <>

#ifndef IN_IDE_PARSERER
#include "session.t++"
#endif


#endif
