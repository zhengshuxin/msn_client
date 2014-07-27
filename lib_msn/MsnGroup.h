#pragma once
#include <list>
#include <set>
#include "lib_msn.h"

class acl::charset_conv;
class CMsnContact;

class LIB_MSN_API CMsnGroup
{
public:
	CMsnGroup(ACL_XML_NODE* group, acl::charset_conv& conv);
	~CMsnGroup(void);

	char* groupId;
	bool propertiesChanged;
	bool fDeleted;
	char* lastChange;

	// of groupInfo
	char* groupType;
	char* name;
	bool IsNotMobileVisible;
	bool IsPrivate;
	bool IsFavorite;

	// 添加从属于该组的成员
	bool AddContact(CMsnContact* contact);

	// 输出该组的信息
	void Out(void) const;
private:
	// 从属于该组的用户成员列表
	std::set<CMsnContact*> contacts_;
};

struct ACL_ARRAY;

class LIB_MSN_API CMsnGroups
{
public:
	CMsnGroups(void);
	~CMsnGroups(void);

	static CMsnGroups* Create(ACL_ARRAY* groupsArray);

	std::list<CMsnGroup*>& GetGroups(void)
	{
		return (groups_);
	}

	// 根据组ID查找组对象
	CMsnGroup* GetGroup(const char* guid) const;

	void Out(void) const;
protected:
private:
	std::list<CMsnGroup*> groups_;
};
