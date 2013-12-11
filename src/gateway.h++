#ifndef gateway_h
#define gateway_h

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/concept_check.hpp>
#include <tuple>
#include "session.h++"
#include "encryption.h++"

#include <list>
#include <set>
#include <map>
#include <cstdint>


namespace {

//! gateway the class which connects to other servers
template <typename Server>
class gateway {
public:
	using protocol_type = typename Server::gateway_protocol_type;
	using netdevice_type = typename Server::netdevice_type;
	using prefix_type = typename Server::prefix_type;
	using socket_type = typename protocol_type::socket;
	using endpoint_type = typename protocol_type::endpoint;
	using resolver_type = typename protocol_type::resolver;
	using query_type = typename resolver_type::query;
	using session_type = session<gateway>;
	using sessions_endpoint_type = std::map<endpoint_type, session_type*>;
	using sessions_prefix_type = std::map<prefix_type, session_type*>;
	using prefix_handle_type = typename sessions_prefix_type::const_iterator;
	using endpoint_handle_type = typename sessions_endpoint_type::const_iterator;

	//! create sockets and stuff
	explicit gateway(boost::asio::io_service& s, netdevice_type& n);
	~gateway();

	// Handler are a better choice here
	//! connect prefix (called by session)
	void connect(prefix_type const& p, session_type& s);
	//! connect endpoint (called by session)
	void connect(endpoint_type const& e, session_type& s);
	//! disconnect prefix (called by session)
	void disconnect(prefix_handle_type const& h);
	//! disconnect endpoint (called by session)
	void disconnect(endpoint_handle_type const& h);

	//! start event loop of all threads
	void run();
	//! get prefix of this gateway
	prefix_type prefix() const;
	//! get full fingerprint of this gateway
	std::vector<std::uint8_t> const& fingerprint() const;

	//! get the credentials
	gnutls::credentials const& credentials();

	//! insert list of endpoints under a prefix
	void insert(prefix_type const, std::set<endpoint_type> const&);
	//! insert insert list of endpoints (determine prefix automatically)
	void insert(std::set<endpoint_type> const&);
	//! remove endpoint (session get removed too)
	void remove(endpoint_type const&);
	//! forward data to other peers
	void forward(boost::asio::const_buffers_1);
	//! send data using a socket
	std::size_t send(boost::asio::const_buffers_1, endpoint_type, boost::system::error_code&);
	//! receive data on unconnected socket
	void receive(std::shared_ptr<std::vector<std::uint8_t>>, std::shared_ptr<endpoint_type>, boost::system::error_code const&, std::size_t bytes_transfered);
	//! called then a session has decrypted data
	void decrypted(boost::asio::const_buffers_1);
	//! list of all peers
	std::list<endpoint_type> peers();

protected:
	//! autoconnect
	void autoconnect_resolve();

	boost::asio::io_service& io_; //!< the proactor
	socket_type socket_; //!< the unconnected udp socket
	netdevice_type& netdevice_; //!< netdevice to submit data to
	gnutls::credentials credentials_; //<! credentials
	gnutls::cookie cookie_; //!< cookie
	std::mutex socket_mutex_; //!< mutex to protect socket
	std::vector<std::uint8_t> fingerprint_; //!< fingerprint of gateway
	prefix_type prefix_; //<! prefix of gateway
	std::mutex sessions_endpoint_mutex_; //!< mutex to protected endpoints map
	sessions_endpoint_type sessions_endpoint_; //!< map of endpoints->session*
	std::mutex sessions_prefix_mutex_; //!< mutex to protect prefix map
	sessions_prefix_type sessions_prefix_; //!< map of prefixes->session*
	bool link_local_; //!< are we link local?

private:
	explicit gateway() = delete;
	explicit gateway(gateway&&) = delete;
	explicit gateway(gateway const&) = delete;
	gateway& operator=(gateway&&) = delete;
	gateway& operator=(gateway const&) = delete;
};

} // namespace: <>

#ifndef IN_IDE_PARSER
#include "gateway.t++"
#endif

#endif
