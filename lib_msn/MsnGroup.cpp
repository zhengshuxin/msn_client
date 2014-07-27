#include "stdafx.h"
#include "lib_acl.h"
#include "lib_acl.hpp"

#include "Util.h"
#include "MsnContacts.h"
#include "MsnMemberShips.h"
#include ".\msngroup.h"

CMsnGroup::CMsnGroup(ACL_XML_NODE* group, acl::charset_conv& conv)
{
	STRING_SET_MEPTY(groupId);
	STRING_SET_MEPTY(lastChange);
	STRING_SET_MEPTY(groupType);
	STRING_SET_MEPTY(name);

	ACL_XML xml;
	acl_xml_foreach_init(&xml, group);

	groupId = GetFirstText(&xml, "groupId");

	propertiesChanged = GetFirstBool(&xml, "propertiesChanged_");
	fDeleted = GetFirstBool(&xml, "fDeleted");
	lastChange = GetFirstText(&xml, "lastChange");

	groupType = GetFirstText(&xml, "groupInfo/groupType");

	acl::string buf;
	name = GetFirstText(&xml, "groupInfo/name");
	if (!STRING_IS_EMPTY(name) &&
		conv.convert("utf-8", "gb18030", name, strlen(name), &buf) == true)
	{
		STRING_SAFE_FREE(name);
		name = acl_mystrdup(buf.c_str());
	}

	IsNotMobileVisible = GetFirstBool(&xml, "groupInfo/IsNotMobileVisible");
	IsPrivate = GetFirstBool(&xml, "groupInfo/IsPrivate");
	IsFavorite = GetFirstBool(&xml, "groupInfo/IsFavorite");
}

CMsnGroup::~CMsnGroup(void)
{
	STRING_SAFE_FREE(groupId);
	STRING_SAFE_FREE(lastChange);
	STRING_SAFE_FREE(groupType);
	STRING_SAFE_FREE(name);
}

bool CMsnGroup::AddContact(CMsnContact* contact)
{
	if (STRING_IS_EMPTY(contact->passportName))
	{
		logger_error("passportName null");
		return (false);
	}
	std::set<CMsnContact*>::iterator it = contacts_.find(contact);
	if (it != contacts_.end())
	{
		logger_warn("member(%s) already exist in group(%s)",
			contact->passportName, name);
		return (false);
	}
	contacts_.insert(contact);
	return (true);
}

void CMsnGroup::Out(void) const
{
	printf(">> CMsnGroup: group name: %s, id: %s\n", name, groupId);

	std::set<CMsnContact*>::const_iterator cit = contacts_.begin();
	for (; cit != contacts_.end(); cit++)
		(*cit)->Out();
}

//////////////////////////////////////////////////////////////////////////

CMsnGroups::CMsnGroups(void)
{

}

CMsnGroups::~CMsnGroups(void)
{
	std::list<CMsnGroup*>::iterator it = groups_.begin();
	for (; it != groups_.end(); it++)
		delete (*it);
	groups_.clear();
}

CMsnGroups* CMsnGroups::Create(ACL_ARRAY* groupsArray)
{
	acl::charset_conv conv;
	CMsnGroups* groups = NEW CMsnGroups();
	ACL_ITER iter;
	acl_foreach(iter, groupsArray)
	{
		ACL_XML_NODE* node = (ACL_XML_NODE*) iter.data;
		CMsnGroup* group = NEW CMsnGroup(node, conv);
		groups->groups_.push_back(group);
	}

	return (groups);
}

CMsnGroup* CMsnGroups::GetGroup(const char* guid) const
{
	if (STRING_IS_EMPTY(guid))
	{
		logger_error("guid null");
		return (NULL);
	}

	std::list<CMsnGroup*>::const_iterator it = groups_.begin();
	for (; it != groups_.end(); it++)
	{
		// sanity check
		if (STRING_IS_EMPTY((*it)->groupId))
			continue;
		if (strcasecmp((*it)->groupId, guid) == 0)
			return (*it);
	}
	return (NULL);
}

void CMsnGroups::Out(void) const
{
	printf(">>>>>>>>>>>>>>>>>>groups<<<<<<<<<<<<<<<<<<<\n");

	std::list<CMsnGroup*>::const_iterator cit = groups_.begin();
	for (; cit != groups_.end(); cit++)
		(*cit)->Out();
}