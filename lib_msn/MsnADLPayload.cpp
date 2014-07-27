#include "stdafx.h"
#include <list>
#include "MsnMemberShips.h"
#include "MsnClient.h"
#include ".\msnadlpayload.h"

struct MSN_DOMAIN
{
	char  name[256];
	std::list<const Member*> members;
};

/*typedef struct _DOMAIN {
	char  name[256];
	std::list<const Member*> members;
} _DOMAIN*/;

CMsnADLPayload::CMsnADLPayload(void)
{
}

CMsnADLPayload::~CMsnADLPayload(void)
{
}

static void AddToDomain(std::list<MSN_DOMAIN*>& domains, const Member* member)
{
	if (member->PassportName == NULL)
		return;

	// 防重检查，如果该 member 已经存在，则返回

	std::list<MSN_DOMAIN*>::const_iterator cit0 = domains.begin();
	for (; cit0 != domains.end(); cit0++)
	{
		std::list<const Member*>::const_iterator cit01 =
			(*cit0)->members.begin();
		for (; cit01 != (*cit0)->members.end(); cit01++)
		{
			if (strcasecmp((*cit01)->PassportName,
				member->PassportName) == 0)
			{
				return;
			}
		}
	}

	// 从 PassportName 中提取出域名

	const char* ptr = strchr(member->PassportName, '@');
	if (ptr == NULL)
	{
		logger_warn("PassportName(%s) invalid", member->PassportName);
		return;
	}

	ptr++;
	if (*ptr == 0)
	{
		logger_warn("PassportName(%s) invalid", member->PassportName);
		return;
	}

	char name[256];  // 存储域名缓冲区
	_snprintf(name, sizeof(name), "%s", ptr);

	MSN_DOMAIN* domain = NULL;

	// 获得具有相同域名的结点对象
	std::list<MSN_DOMAIN*>::iterator it = domains.begin();
	for (; it != domains.end(); it++)
	{
		if (strcasecmp((*it)->name, name) == 0)
		{
			domain = *it;
			break;
		}
	}

	if (domain == NULL)
	{
		// 创建新的域名结点对象
		domain = NEW MSN_DOMAIN;
		_snprintf(domain->name, sizeof(domain->name), "%s", name);
		domains.push_back(domain);
	}

	// 将成员对象 member 添加进域名结点对象
	domain->members.push_back(member);
}

static void ClearDomain(std::list<MSN_DOMAIN*>& domains)
{
	std::list<MSN_DOMAIN*>::iterator it = domains.begin();
	for (; it != domains.end(); it++)
	{
		delete (*it);
	}
	domains.clear();
}

static void AddXmlNode(acl::string& buf, const MSN_DOMAIN* domain)
{
	buf << "<d n='" << domain->name << "'>";
	std::list<const Member*>::const_iterator cit =
		domain->members.begin();
	acl::string name;
	int  op;
	//char tmp[32];
	for (; cit != domain->members.end(); cit++)
	{
		name = (*cit)->PassportName;
		char* ptr = strchr(name.c_str(), '@');
		if (ptr)
			*ptr = 0;

		//op = ((*cit)->op | MSN_LIST_FL_OP) & MSN_LIST_OP_MASK;
		op = (*cit)->op_ & MSN_LIST_OP_MASK;
		buf.format_append("<c n='%s' l='%d' t='1'/>", name.c_str(), op);
	}
	buf << "</d>";
}

void CMsnADLPayload::ToString(CMsnClient* client, acl::string& out,
	const std::list<CMembership*>& memberShips)
{
	std::list<MSN_DOMAIN*> domains;
	acl::string buf;

	// 遍历所有的联系人成员

	std::list<CMembership*>::const_iterator cit = memberShips.begin();
	for (; cit != memberShips.end(); cit++)
	{
		const CMembership* memberShip = *cit;

		const std::list<Member*>& members = memberShip->GetMembers();
		std::list<Member*>::const_iterator cit2 = members.begin();
		for (; cit2 != members.end(); cit2++)
			AddToDomain(domains, *cit2);
	}

	buf = "<ml l='1'>";

	std::list<MSN_DOMAIN*>::const_iterator cit2 = domains.begin();
	for (; cit2 != domains.end(); cit2++)
	{
		AddXmlNode(buf, *cit2);
		if (buf.length() >= 4096)
		{
			buf << "</ml>";
			out << "ADL " << client->Sid()
				<< " " << (int) buf.length() << "\r\n";
			out.append(buf);
			buf = "<ml l='1'>";
		}
	}

	//buf.format_append("<d n='live.com'><c n='support.service' t='32'>"
	//	"<s l='2' n='IM' /><s l='2' n='PE' /></c></d>");
	//buf.format_append("<d n='live.com'><c n='support.service' l='2' t='1'/></d>");
	buf << "</ml>";
	out << "ADL " << client->Sid() << " " << (int) buf.length() << "\r\n";
	out.append(buf);
	ClearDomain(domains);
}