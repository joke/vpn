#ifndef client_h
#define client_h

#include <boost/asio/io_service.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include <iostream>

#include "controller_protocol.h++"
#include <boost/asio/ip/address.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <sstream>


//! the client handles the commandline interface to communicate with a running server
template <typename Protocol>
class client {
public:
	//! create client
	explicit client(boost::asio::io_service&, typename Protocol::endpoint const&);
	//! send out a query to list all nodes and print result
	bool node_list();
	//! send out a query to add a node
	bool node_add(std::string const& node);
	//! send out a query to delete a node
	bool node_del(std::string const& node);

protected:
	//! transfer query to server using a unix socket
	void submit(query& query);

	typename Protocol::iostream stream_; //!< the unix socket in stream mode

private:
	explicit client() = delete;
	explicit client(client const&) = delete;
	explicit client(client&&) = delete;
	client& operator=(client const&) = delete;
	client& operator=(client&&) = delete;
};


#ifndef IN_IDE_PARSERER
#include "client.t++"
#endif

#endif
