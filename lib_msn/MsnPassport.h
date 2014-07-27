#pragma once
#include "string.hpp"
#include "lib_msn.h"

class LIB_MSN_API CMsnPassport
{
public:
	CMsnPassport(void);
	~CMsnPassport(void);

	acl::string sid_;
	acl::string mspauth_;
	acl::string client_ip_;
	unsigned short client_port;
	time_t login_time_;
	bool email_enable_;
	acl::string nickname_;
	acl::string route_info_;
};
