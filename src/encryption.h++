#ifndef encryption_h
#define encryption_h

#include <forward_list>
#include <cstdint>
#include <system_error>
#include <gnutls/gnutls.h>
#include <gnutls/dtls.h>
#include <gnutls/openpgp.h>
#include <boost/asio/buffer.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio.hpp>
#include <gpgme.h>

#include "program_options.h++"

namespace gnutls {
namespace {

//! enum with all the gnutls error codes
enum class errc;

//! gnutls error category
class error_category : public std::error_category {
public:
	error_category(error_category const&) = delete;
	virtual const char* name() const noexcept;
	virtual std::string message(int const) const noexcept;
	static error_category const& instance();

private:
	error_category() = default;
};

//! default error category
std::error_category const& gnutls_category() noexcept;

//! early singleton to call gnutls before main
class global {
private:
	explicit global();
	~global();

	static global* global_;
};

} // namespace: <>

namespace openpgp {
namespace {

class certificate;
class privatekey;

} // namespace: <>
} // namespace: openpgp

namespace {

//! wrapper around gnutls_datum_t
class datum {
public:
	explicit datum();
	explicit datum(std::string const& filename);
	explicit datum(gnutls_datum_t const*);
	explicit datum(std::size_t);
	~datum();
	operator gnutls_datum_t*();
	gnutls_datum_t const& data() const;

protected:
	gnutls_datum_t datum_;
	gnutls_datum_t const* const datum_p_;

private:
	explicit datum(datum&&) = delete;
	explicit datum(datum const&) = delete;
	datum& operator=(datum const&) = delete;
	datum& operator=(datum&&) = delete;
};

class credentials {
public:
	explicit credentials();
	explicit credentials(openpgp::certificate&, openpgp::privatekey&);
	~credentials();
	operator gnutls_certificate_credentials_t() const;
	operator gnutls_priority_t() const;
	//! set verify function
	void verify_function(gnutls_certificate_verify_function);
	//! verify callback function used by gntusl
	static int verify(gnutls_session_t);
	credentials& operator=(credentials&&);

protected:
	gnutls_certificate_credentials_t credentials_; //!< credentials
	gnutls_dh_params_t dh_params_; //!< diffie-hellmann parameter
	gnutls_priority_t priority_; //! priorities

private:
	explicit credentials(credentials const&) = delete;
	explicit credentials(credentials&&) = delete;
	credentials& operator=(credentials const&) = delete;
};

//! wrapper around gnutls's dtls cookie
class cookie {
public:
	cookie(std::size_t);
	operator gnutls_dtls_prestate_st*();

	template <typename T>
	bool verify(T& clientData, boost::asio::const_buffers_1);
	void reset();

	//! push functor used to push data
	template <typename PushFunctor>
	bool send(PushFunctor p, boost::asio::const_buffers_1 identification_data);

protected:
	gnutls_datum_t key_;
	gnutls_dtls_prestate_st prestate_;
};

} // namespace: <>

namespace openpgp {
namespace {

//! openpgp certificate in gnutls
class certificate {
public:
	explicit certificate(datum const&);
	~certificate();
	operator gnutls_openpgp_crt_t() const;
	//! get vector with fingerprint
	std::vector<std::uint8_t> fingerprint() const;
	//! set preferred subkey
	void preferred(std::array<std::uint8_t, 8> const&);
	//! get auth subkey
	std::array<std::uint8_t, 8> authsubkey() const;

protected:
	gnutls_openpgp_crt_t certificate_;

private:
	explicit certificate() = delete;
	explicit certificate(certificate const&) = delete;
	explicit certificate(certificate&&) = delete;
	certificate& operator=(certificate const&) = delete;
	certificate& operator=(certificate&&) = delete;
};

//! openpgp private key in gnutls
class privatekey {
public:
	explicit privatekey(datum const&);
	~privatekey();
	operator gnutls_openpgp_privkey_t() const;
	//! get vector with fingerprint
	std::vector<std::uint8_t> fingerprint() const;
	//! set preferred subkey
	void preferred(std::array<std::uint8_t, 8> const&);

protected:
	gnutls_openpgp_privkey_t privatekey_;

private:
	explicit privatekey() = delete;
	explicit privatekey(privatekey const&) = delete;
	explicit privatekey(privatekey&&) = delete;
	privatekey& operator=(privatekey const&) = delete;
	privatekey& operator=(privatekey&&) = delete;
};

} // namespace: <>
} // namespace: openpgp
} // namespace: gnutls

namespace gpgme {
namespace {

//! wrapper around gpgme data
class data {
public:
	explicit data();
	explicit data(char const* const, std::size_t, bool copy = false);
	~data();
	operator gpgme_data_t();
	std::size_t size() const;

protected:
	gpgme_data_t data_;

private:
	explicit data(data const&) = delete;
	explicit data(data&&) = delete;
	data& operator=(data const&) = delete;
	data& operator=(data&&) = delete;
};

//! openpgp key in gpgme
class key {
public:
	explicit key();
	~key();
	operator gpgme_key_t();
	operator gpgme_key_t*();

protected:
	gpgme_key_t key_;

private:
	explicit key(key&&) = delete;
	explicit key(key const&) = delete;
	key& operator=(key const&) = delete;
	key& operator=(key&&) = delete;
};

//! gpgme context
class context {
public:
	explicit context();
	explicit context(context&&);
	~context();
	operator gpgme_ctx_t();

	//! import key into keyring (send during handshake)
	bool import_key(data&);
	//! import key from key (as returned keyserver)
	bool import_key(gpgme::key const&);
	//! export key into data
	bool export_key(data&, char const* const pattern) const;
	//! get key from local keysring or query external server
	bool key(gpgme::key&, char const* const pattern, bool external = false) const;
	//! list all peers with vpn uids.
	void vpn_peers(std::forward_list<std::tuple<std::string, std::set<boost::asio::ip::udp::endpoint>>>& peers);

protected:
	gpgme_ctx_t context_;

private:
	explicit context(context const&) = delete;
	context& operator=(context const&) = delete;
	context& operator=(context&&) = delete;
};

} // namespace: <>
} // namespace: gpgme


namespace std {

template <>
struct is_error_condition_enum<gnutls::errc> : public true_type {
};

error_code make_error_code(gnutls::errc);

} // namespace: std

#ifndef IN_IDE_PARSERER
#include "encryption.t++"
#endif

#endif


