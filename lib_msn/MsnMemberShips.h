#pragma once
#include <list>
#include <set>
#include "string.hpp"
#include "MsnUserList.h"
#include "lib_msn.h"

// 联系人的在线状态
typedef enum
{
	member_s_offline,
	member_s_online,
	member_s_idle,
	member_s_away,
	member_s_busy,
} MemberStatus;

LIB_MSN_API const char* StatusToString(MemberStatus status);

//class Member;
//class MemberCmp
//{
//public:
//	MemberCmp();
//	~MemberCmp();
//
//	bool operator()(const Member* a, const Member* b) const;
//protected:
//private:
//};

class CMsnContact;

// 联系人成员结果类型定义
class LIB_MSN_API Member
{
public:
	Member();
	~Member();

	// from xml data

	// for PassportName
	char* DisplayName;   // 显示名
	char* PassportName;  // 帐号
	bool  IsPassportNameHidden;
	int   PassportId;
	char* CID;
	bool  LookedupByCID;

	// for Email
	char* Email;  // 邮箱地址

	// for Phone
	char* Phone;

	// 通用项
	int   MembershipId;
	char* Type;
	char* State;
	bool  Deleted;
	char* LastChanged;
	char* JoinedDate;
	char* ExpirationDate;

	// added
	MsnListOp  op_;
	bool authorized_;       // 邮箱地址是否是已认证
	MemberStatus online_;   // 在线状态

	CMsnContact* contact_;  // 关联的联系人对象
	void SetContact(CMsnContact* contact);

	// 输出该成员的信息
	void Out(void) const;
};

struct ACL_XML;
class acl::charset_conv;

class LIB_MSN_API CMembership
{
public:
	/**
	* 构造函数，根据传入的含有 MemberShip 数据的 xml 结点，分析出
	* 该 MemberShip 中的成员变量
	* @param xmlMemberShip {ACL_XML&} 某个 MemberShip 结点以及其
	*  子结点组成的 xml 树对象
	* @param conv {acl::charset_conv*} 如果非空，由对解析结果用该
	*  转码器进行字符集的转换
	*/
	CMembership(ACL_XML& xmlMembership, acl::charset_conv* conv);
	~CMembership(void);

	// 返回值需要用 STRING_IS_EMPTY 判断是否是空值
	const char* GetMemberRole() const
	{
		return MemberRole_;
	}

	const std::list<Member*> GetMembers(void) const
	{
		return members_;
	}

	MsnListId GetListId() const
	{
		return listId_;
	}

	MsnListOp GetListOp() const
	{
		return listOp_;
	}

	void Out(void) const;
private:
	char* MemberRole_;
	MsnListId listId_;
	MsnListOp listOp_;
	std::list<Member*> members_;

	/**
	* 分析 XML 数据结点，产生 Member 对象
	* @param xmlMember {ACL_XML*} 以 Member 结点为根的 XML 树
	* @param conv {acl::charset_conv*} 字符集转码器，非空时则用该
	*  转码器进行字符集转换
	* @param listOp {MsnListOp}
	*/
	void AddMember(ACL_XML* xmlMember, acl::charset_conv* conv, MsnListOp listOp);

	/**
	* 设置成员的通用属性
	* @param xmlMember {ACL_XML*} 以 Member 结点为根的 XML 树
	* @param member {Member*} 联系人成员
	*/
	void SetMemberCommon(ACL_XML* xmlMember, Member* member);

	/**
	* 获得 Passport 类型的联系人成员对象
	* @param xmlMember {ACL_XML*} 以 Member 结点为根的 XML 树
	* @param conv {acl::charset_conv*} 字符集转码器
	* @return {Member*} Passport 类型的联系人对象
	*/
	Member* GetPassportMember(ACL_XML* xmlMember, acl::charset_conv* conv);

	/**
	* 获得 Email 类型的成员对象
	* @param xmlMember {ACL_XML*} 以 Member 结点为根的 XML 树
	* @return {Member*} Email 类型的联系人对象
	*/
	Member* GetEmailMember(ACL_XML* xmlMember);

	/**
	* 获得 Phone 类型的成员对象
	* @param xmlMember {ACL_XML*} 以 Member 结点为根的 XML 树
	* @return {Member*} Phone 类型的联系人对象
	*/
	Member* GetPhoneMember(ACL_XML* xmlMember);
};

struct ACL_ARRAY;

class LIB_MSN_API CMsnMemberShips
{
public:
	CMsnMemberShips(void);
	~CMsnMemberShips(void);

	/**
	* 根据传入的 MemberShip xml 结点集合创建 MemberShip 集合对象,
	* 创建的对象需要 delete 掉
	* @param memberShipArray {ACL_ARRAY*} 由 ACL_XML_NODE 组成的数组，每个XML
	*  结点对象提供了一个 Membership 的原始数据信息
	* @return {CMsnMemberShipContacts*} 用完后需要 delete 释放
	*/
	static CMsnMemberShips* Create(ACL_ARRAY* memberShipArray);

	// 获得所有的 Membership 集合列表
	std::list<CMembership*>& GetMemberShips(void);

	// 获得需要显示的用户列表
	//std::set<Member*, MemberCmp>& GetUsers(void);
	//std::list<Member*>& GetUsers(void);

	// 调试接口：输出所有联系人列表
	void Out(void) const;
private:
	std::list<CMembership*> memberships_;
	//std::set<Member*, MemberCmp> users_;
	//std::list<Member*> users_;
	//bool user_builded_;
};
