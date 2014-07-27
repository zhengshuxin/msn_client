#pragma once
#include <list>
#include <set>
#include "string.hpp"
#include "lib_msn.h"

class acl::charset_conv;
struct ACL_XML_NODE;
class CMsnGroups;
class CMsnGroup;

class LIB_MSN_API CMsnContact
{
public:
	CMsnContact(ACL_XML_NODE* contact, acl::charset_conv& conv,
		CMsnGroups* groups);
	~CMsnContact(void);

	void Out(void) const;

	char* contactId;
	bool  propertiesChanged;
	bool  fDeleted;
	char* CreateDate;
	char* lastChange;

	// contactInfo

	std::list<acl::string> groupIds;  // 所属组的组ID集合
	char* contactType;
	char* quickName;
	char* passportName;
	bool  IsPassportNameHidden;
	char* displayName;
	int   puid;
	char* CID;
	bool  IsNotMobileVisible;
	bool  isMobileIMEnabled;
	bool  isMessengerUser;
	bool  isFavorite;
	bool  isSmtp;
	bool  hasSpace;
	char* spotWatchState;
	char* birthdate;
	char* primaryEmailType;
	char* PrimaryLocation;
	char* PrimaryPhone;
	bool  IsPrivate;
	bool  IsHidden;
	char* Gender;
	char* TimeZone;
	bool  IsAutoUpdateDisabled;
	bool  IsShellContact;
	int   TrustLevel;
	bool  PropertiesChanged;

	const std::set<CMsnGroup*>& GetGroups() const
	{
		return groups_;
	}
private:
	// added
	std::set<CMsnGroup*> groups_;  // 所属组集合

	// 添加本用户所属的组，有可能同属于多个组
	bool AddGroup(CMsnGroup* group);
};

struct ACL_ARRAY;

class CMsnContacts
{
public:
	CMsnContacts();
	~CMsnContacts();

	static CMsnContacts* Create(ACL_ARRAY* contactArray, CMsnGroups* groups);

	std::list<CMsnContact*>& GetContacts(void)
	{
		return (contacts_);
	}

	void Out(void) const;
public:
protected:
private:
	std::list<CMsnContact*> contacts_;
};