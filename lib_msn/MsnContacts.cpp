#include "stdafx.h"
#include "lib_acl.h"
#include "lib_acl.hpp"
#include "Util.h"
#include "MsnGroup.h"
#include ".\MsnContacts.h"

CMsnContact::CMsnContact(ACL_XML_NODE* contact,
	acl::charset_conv& conv, CMsnGroups* groups)
{
	STRING_SET_MEPTY(contactId);
	STRING_SET_MEPTY(CreateDate);
	STRING_SET_MEPTY(lastChange);
	STRING_SET_MEPTY(contactType);
	STRING_SET_MEPTY(quickName);
	STRING_SET_MEPTY(passportName);
	STRING_SET_MEPTY(displayName);
	STRING_SET_MEPTY(CID);
	STRING_SET_MEPTY(spotWatchState);
	STRING_SET_MEPTY(birthdate);
	STRING_SET_MEPTY(primaryEmailType);
	STRING_SET_MEPTY(PrimaryLocation);
	STRING_SET_MEPTY(PrimaryPhone);
	STRING_SET_MEPTY(Gender);
	STRING_SET_MEPTY(TimeZone);

	ACL_XML xml;
	acl_xml_foreach_init(&xml, contact);

	contactId = GetFirstText(&xml, "GetFirstText");
	propertiesChanged = GetFirstBool(&xml, "propertiesChanged");
	fDeleted = GetFirstBool(&xml, "fDeleted");
	CreateDate = GetFirstText(&xml, "CreateDate");
	lastChange = GetFirstText(&xml, "lastChange");

	// contactInfo

	acl::string buf;

	contactType = GetFirstText(&xml, "contactInfo/contactType");

	quickName = GetFirstText(&xml, "contactInfo/quickName");
	if (!STRING_IS_EMPTY(quickName) &&
		conv.convert("utf-8", "gb18030", quickName,
			strlen(quickName), &buf) == true)
	{
		STRING_SAFE_FREE(quickName);
		quickName = acl_mystrdup(buf.c_str());
	}

	passportName = GetFirstText(&xml, "contactInfo/passportName");
	IsPassportNameHidden = GetFirstBool(&xml, "contactInfo/IsPassportNameHidden");

	displayName = GetFirstText(&xml, "contactInfo/displayName");
	if (!STRING_IS_EMPTY(displayName) &&
		conv.convert("utf-8", "gb18030", displayName,
			strlen(displayName), &buf) == true)
	{
		STRING_SAFE_FREE(displayName);
		displayName = acl_mystrdup(buf.c_str());
	}

	puid = GetFirstInt(&xml, "contactInfo/puid");
	CID = GetFirstText(&xml, "contactInfo/CID");
	IsNotMobileVisible = GetFirstBool(&xml, "contactInfo/IsNotMobileVisible");
	isMobileIMEnabled = GetFirstBool(&xml, "contactInfo/isMobileIMEnabled");
	isMessengerUser = GetFirstBool(&xml, "contactInfo/isMessengerUser");
	isFavorite = GetFirstBool(&xml, "contactInfo/isFavorite");
	isSmtp = GetFirstBool(&xml, "contactInfo/isSmtp");
	hasSpace = GetFirstBool(&xml, "contactInfo/hasSpace");
	spotWatchState = GetFirstText(&xml, "spotWatchState");
	birthdate = GetFirstText(&xml, "contactInfo/birthdate");
	primaryEmailType = GetFirstText(&xml, "contactInfo/primaryEmailType");
	PrimaryLocation = GetFirstText(&xml, "contactInfo/PrimaryLocation");
	PrimaryPhone = GetFirstText(&xml, "contactInfo/PrimaryPhone");
	IsPrivate = GetFirstBool(&xml, "contactInfo/IsPrivate");
	IsHidden = GetFirstBool(&xml, "contactInfo/IsHidden");
	Gender = GetFirstText(&xml, "contactInfo/Gender");
	TimeZone = GetFirstText(&xml, "contactInfo/TimeZone");
	IsAutoUpdateDisabled = GetFirstBool(&xml, "contactInfo/IsAutoUpdateDisabled");
	IsShellContact = GetFirstBool(&xml, "contactInfo/IsShellContact");
	TrustLevel = GetFirstInt(&xml, "TcontactInfo/rustLevel");
	PropertiesChanged = GetFirstBool(&xml, "contactInfo/PropertiesChanged");

	// 列出该联系人所从属的所有组的组ID号
	ACL_ARRAY* a = acl_xml_getElementsByTags(&xml, "contactInfo/groupIds/guid");
	if (a)
	{
		ACL_ITER iter;
		acl_foreach(iter, a)
		{
			ACL_XML_NODE* node = (ACL_XML_NODE*) iter.data;
			if (node->text == NULL || ACL_VSTRING_LEN(node->text) == 0)
				continue;
			const char* guid = acl_vstring_str(node->text);
			groupIds.push_back(guid);

			if (groups == NULL)
				continue;
			// 查找对应组ID -- guid 的组对象
			CMsnGroup* group = groups->GetGroup(guid);
			if (group == NULL)
				continue;
			// 将该组对象添加进本联系人所在的组对象集合中
			AddGroup(group);
		}
	}
}

CMsnContact::~CMsnContact(void)
{
	STRING_SAFE_FREE(contactId);
	STRING_SAFE_FREE(CreateDate);
	STRING_SAFE_FREE(lastChange);
	STRING_SAFE_FREE(contactType);
	STRING_SAFE_FREE(quickName);
	STRING_SAFE_FREE(passportName);
	STRING_SAFE_FREE(displayName);
	STRING_SAFE_FREE(CID);
	STRING_SAFE_FREE(spotWatchState);
	STRING_SAFE_FREE(birthdate);
	STRING_SAFE_FREE(primaryEmailType);
	STRING_SAFE_FREE(PrimaryLocation);
	STRING_SAFE_FREE(PrimaryPhone);
	STRING_SAFE_FREE(Gender);
	STRING_SAFE_FREE(TimeZone);
}

bool CMsnContact::AddGroup(CMsnGroup* group)
{
	if (STRING_IS_EMPTY(group->name))
	{
		logger_error("group's name null");
		return (false);
	}
	std::set<CMsnGroup*>::iterator it = groups_.find(group);
	if (it != groups_.end())
	{
		logger_error("group(%s) already added in member(%s)",
			group->name, passportName ? passportName : "unkown");
		return (false);
	}
	groups_.insert(group);
	group->AddContact(this);
	return (true);
}

void CMsnContact::Out(void) const
{
	printf(">> CMsnContact:: contact dispaly name: %s, passport name: %s\n",
		displayName, passportName);
}

//////////////////////////////////////////////////////////////////////////

CMsnContacts::CMsnContacts(void)
{

}

CMsnContacts::~CMsnContacts(void)
{
	std::list<CMsnContact*>::iterator it = contacts_.begin();
	for (; it != contacts_.end(); it++)
		delete (*it);
	contacts_.clear();
}

CMsnContacts* CMsnContacts::Create(ACL_ARRAY* contactArray, CMsnGroups* groups)
{
	acl::charset_conv conv;
	CMsnContacts* contacts = NEW CMsnContacts();
	ACL_ITER iter;
	acl_foreach(iter, contactArray)
	{
		ACL_XML_NODE* node = (ACL_XML_NODE*) iter.data;
		CMsnContact* contact = NEW CMsnContact(node, conv, groups);
		contacts->contacts_.push_back(contact);
	}

	return (contacts);
}

void CMsnContacts::Out(void) const
{
	printf(">>>>>>>>>>>>>>>>contacts<<<<<<<<<<<<<<<<<<\n");
	std::list<CMsnContact*>::const_iterator cit = contacts_.begin();
	for (; cit != contacts_.end(); cit++)
		(*cit)->Out();
}
