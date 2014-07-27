#pragma once
#include <list>
#include "string.hpp"
#include "MsnContactManager.h"
#include "lib_msn.h"

class CMembership;
class CMsnClient;

class LIB_MSN_API CMsnADLPayload
{
public:
	CMsnADLPayload(void);
	~CMsnADLPayload(void);

	static void ToString(CMsnClient* client, acl::string& buf,
		const std::list<CMembership*>& memberShips);
private:
};
