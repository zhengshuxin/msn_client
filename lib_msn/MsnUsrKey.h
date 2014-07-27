#pragma once
#include "lib_msn.h"

class CMsnTicket;
class acl::string;

class LIB_MSN_API CMsnUsrKey
{
public:
	CMsnUsrKey(void);
	~CMsnUsrKey(void);

	bool CreateKey(const char* secret, const char* nonce, acl::string& buf);
};
