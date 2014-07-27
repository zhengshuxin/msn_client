#pragma once
#include "lib_msn.h"
#include "lib_acl.h"
#include "string.hpp"
#include "ofstream.hpp"

class CMsnTicket;
class acl::ofstream;

class LIB_MSN_API CMsnSSO
{
public:
	CMsnSSO(const char* username, const char* password, const char* sso_policy);
	~CMsnSSO(void);

	CMsnTicket* GetTicket();
private:
	acl::string m_username;
	acl::string m_password;
	acl::string m_ssoPolicy;
	size_t  ticket_domain_count_;

	void BuildRequest(acl::string& request);
	void TlsInit();
	bool ParsePassport(CMsnTicket* ticket, ACL_XML_NODE* node, const char* addr);
	bool ParseDomain(CMsnTicket* ticket, ACL_XML_NODE* node, const char* addr);
	bool ParseResponse(CMsnTicket* ticket, ACL_XML_NODE* node);
	bool ParseCollection(CMsnTicket* ticket, ACL_ARRAY* collections);

	acl::ofstream* debug_fpout_;
	void logger_format(const char* fmt, ...);
};
