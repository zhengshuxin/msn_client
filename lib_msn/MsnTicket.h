#pragma once
#include "lib_msn.h"
#include "string.hpp"

struct TICKET
{
	char* id;
	char* domain;
	char* policy;
	char* secret;
	char* expires;
	char* ticket;
	char* p;
};

class LIB_MSN_API CMsnTicket
{
public:
	CMsnTicket(void);
	~CMsnTicket(void);

	void AddTicket(const char* id, const char* domain,
		const char* secret, const char* expires, const char* txt);
	void SetCipher(const char* cipher);
	void SetSecret(const char* secret);
	const TICKET* GetTicket(const char* domain) const;
private:
	std::list<TICKET*> tickets_;
	acl::string cipher_;
	acl::string secret_;
};
