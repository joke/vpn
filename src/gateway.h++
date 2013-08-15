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
	typedef typename Server::gateway_protocol_type protocol_type;
	typedef typename Server::netdevice_type netdevice_type;
	typedef typename Server::prefix_type prefix_type;
	typedef typename protocol_type::socket socket_type;
	typedef typename protocol_type::endpoint endpoint_type;
	typedef typename protocol_type::resolver resolver_type;
	typedef typename resolver_type::query query_type;
	typedef session<gateway> session_type;
	typedef std::map<endpoint_type, session_type*> sessions_endpoint_type;
	typedef std::map<prefix_type, session_type*> sessions_prefix_type;
	typedef typename sessions_prefix_type::const_iterator prefix_handle_type;
	typedef typename sessions_endpoint_type::const_iterator endpoint_handle_type;

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
