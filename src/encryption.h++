#ifndef encryption_h
#define encryption_h

#include <gnutls/gnutls.h>
#include <gnutls/dtls.h>
#include <gnutls/openpgp.h>
#include <system_error>
#include <vector>
#include <array>

#include <string>

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
	datum(std::vector<std::uint8_t> const&);
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

class diffie_hellman {
public:
	explicit diffie_hellman(diffie_hellman&&);
	explicit diffie_hellman(std::size_t);
	~diffie_hellman();
	diffie_hellman& operator=(diffie_hellman&&);
	operator gnutls_dh_params_t() const;

protected:
	::gnutls_dh_params_t diffie_hellman_;

private:
	explicit diffie_hellman(diffie_hellman const&) = delete;
	diffie_hellman& operator=(diffie_hellman const&) = delete;
};

class priorities {
public:
	explicit priorities(priorities&&) {}
	explicit priorities(std::string const&);
	~priorities();
	priorities& operator=(priorities&&) { return *this; }
	operator gnutls_priority_t() const;

protected:
	::gnutls_priority_t priorities_;

private:
	explicit priorities(priorities const&) = delete;
	priorities& operator=(priorities const&) = delete;
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

namespace std {

template <>
class is_error_condition_enum<gnutls::errc> : public true_type {
};

error_code make_error_code(gnutls::errc);

} // namespace: std


#ifndef IN_IDE_PARSERER
#include "encryption.t++"
#endif

#endif
