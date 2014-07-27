#pragma once
#include "string.hpp"
#include "lib_msn.h"

class LIB_MSN_API CMsnClientInfo
{
public:
	CMsnClientInfo(void);
	~CMsnClientInfo(void);

	acl::string msn_protocol_;
	acl::string product_id_;
	acl::string product_key_;
	acl::string client_name_;
	acl::string build_ver_;
	acl::string application_id_;
	acl::string client_brand_;
};
