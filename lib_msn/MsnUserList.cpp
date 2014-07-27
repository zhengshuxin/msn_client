#include "stdafx.h"
#include "Util.h"
#include ".\msnuserlist.h"

CMsnUserList::CMsnUserList(void)
{
}

CMsnUserList::~CMsnUserList(void)
{
}

MsnListId msn_get_memberrole(const char *role)
{
	if (STRING_IS_EMPTY(role))
		return MSN_LIST_NL;
	else if (!strcmp(role,"Allow"))
		return MSN_LIST_AL;
	else if (!strcmp(role,"Block"))
		return MSN_LIST_BL;
	else if (!strcmp(role,"Reverse"))
		return MSN_LIST_RL;
	else if (!strcmp(role,"Pending"))
		return MSN_LIST_PL;

	return MSN_LIST_NL;
}