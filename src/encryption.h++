#ifndef encryption_h
#define encryption_h

#include <vector>
#include <system_error>
#include <boost/config/no_tr1/complex.hpp>
#include <gnutls/gnutls.h>
#include <gnutls/openpgp.h>
#include <gnutls/dtls.h>


namespace gnutls {

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

public:
	static void debug(unsigned short const level);
};

namespace openpgp {

class certificate {
public:
	explicit certificate() = delete;
	explicit certificate(certificate const&) = delete;
	explicit certificate(std::string const&);
	certificate(certificate&&);
	~certificate();

	certificate const& operator=(certificate const&) = delete;
	operator gnutls_openpgp_crt_t() const;

protected:
	::gnutls_openpgp_crt_t certificate_;
};

class privatekey {
public:
	explicit privatekey() = delete;
	explicit privatekey(privatekey const&) = delete;
	explicit privatekey(std::string const&);
	privatekey(privatekey&&);
	~privatekey();

	privatekey const& operator=(privatekey const&) = delete;
	operator gnutls_openpgp_privkey_t() const;

protected:
	::gnutls_openpgp_privkey_t privatekey_;
};

} // namespace: openpgp

class diffie_hellman_parameters;
class session;

class credentials {
public:
	friend session;

	explicit credentials() = delete;
	explicit credentials(credentials const&) = delete;
	explicit credentials(openpgp::certificate&&, openpgp::privatekey&&);
	credentials(credentials&&);
	~credentials();

	credentials const& operator=(credentials const&) = delete;

	void set(diffie_hellman_parameters const&);

public:
	::gnutls_certificate_credentials_t credentials_;
	openpgp::certificate certificate_;
	openpgp::privatekey privatekey_;
};

class diffie_hellman_parameters {
public:
	friend credentials;

	explicit diffie_hellman_parameters() = delete;
	explicit diffie_hellman_parameters(diffie_hellman_parameters const&) = delete;
	diffie_hellman_parameters(diffie_hellman_parameters&&);
	explicit diffie_hellman_parameters(std::size_t const);
	~diffie_hellman_parameters();

	diffie_hellman_parameters const& operator=(diffie_hellman_parameters const&) = delete;
	diffie_hellman_parameters& operator=(diffie_hellman_parameters&&);

protected:
	::gnutls_dh_params_t parameters_;
};

class priorities {
public:
	friend session;

	explicit priorities() = delete;
	explicit priorities(priorities const&) = delete;
	priorities(priorities&&);
	explicit priorities(std::string const&);
	~priorities();

	priorities const& operator=(priorities const&) = delete;
	priorities const& operator=(priorities&&);

protected:
	::gnutls_priority_t priorities_;
};

class session {
public:
	explicit session() = delete;
	explicit session(session const&) = delete;
	session(session&&);
	explicit session(bool server, credentials const&, priorities const& p);
	~session();

	session const& operator=(session const&) = delete;

protected:
	gnutls_session_t session_;
};

} // namespace: gnutls


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


