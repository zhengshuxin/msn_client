#include "StdAfx.h"
#include "MsnClient.h"
#include "MsnSSO.h"
#include "MsnTicket.h"
#include "MsnNSClient.h"
#include "MsnContactManager.h"
#include ".\msnservice.h"

class sso_request;
class contact_request;

struct MSN_IPC_DAT
{
	sso_request* req_sso;
	contact_request* req_contact;
};

typedef enum
{
	MSN_TICKET = WM_USER + 1,
	MSN_TICKET_ERR,
	MSN_CONTACT,
	MSN_CONTACT_ERR,
} msn_msg_t;

//////////////////////////////////////////////////////////////////////////
// sso 工作线程处理类

class sso_request : public acl::ipc_request
{
public:
	sso_request(CMsnClient* client, const char* account, const char* passwd)
		: client_(client)
		, account_(account)
		, passwd_(passwd)
	{

	}

	~sso_request()
	{

	}

	CMsnTicket* GetTicket() const
	{
		return (ticket_);
	}

	CMsnClient* GetClient() const
	{
		return (client_);
	}
protected:
	// 基类虚接口，使子线程可以在执行完任务后向主线程发送 WIN32 窗口消息
	virtual void run(HWND hWnd)
	{
		MSN_IPC_DAT* data = (MSN_IPC_DAT*) acl_mycalloc(1, sizeof(MSN_IPC_DAT));
		data->req_sso = this;

		CMsnSSO* sso = NEW CMsnSSO(account_.c_str(), passwd_.c_str(), NULL);
		ticket_ = sso->GetTicket();
		delete sso;

		if (ticket_ == NULL)
		{
			logger_error("get user(%s)'s ticket error",
				account_.c_str());
			::PostMessage(hWnd, MSN_TICKET_ERR, 0, (LPARAM) data);
		}
		else
			::PostMessage(hWnd, MSN_TICKET, 0, (LPARAM) data);
	}
private:
	CMsnClient* client_;
	acl::string account_;
	acl::string passwd_;
	CMsnTicket* ticket_;
};
//////////////////////////////////////////////////////////////////////////
// 获得联系人过程

class contact_request : public acl::ipc_request
{
public:
	contact_request(CMsnNSClient* ns_client,
		const CMsnTicket& ticket)
		: ns_client_(ns_client)
		, ticket_(ticket)
	{

	}

	~contact_request()
	{

	}

	CMsnNSClient* GetNsClient() const
	{
		return (ns_client_);
	}
protected:
	// 基类虚接口，使子线程可以在执行完任务后向主线程发送 WIN32 窗口消息
	virtual void run(HWND hWnd)
	{
		MSN_IPC_DAT* data = (MSN_IPC_DAT*)
			acl_mycalloc(1, sizeof(MSN_IPC_DAT));
		data->req_contact = this;

		CMsnContactManager& manager = ns_client_->GetContactManager();
		// 获得联系人及地址簿
		if (manager.GetMessage(ticket_) == true)
			::PostMessage(hWnd, MSN_CONTACT, 0, (LPARAM) data);
		else
			::PostMessage(hWnd, MSN_CONTACT_ERR, 0, (LPARAM) data);
	}
private:
	CMsnNSClient* ns_client_;
	const CMsnTicket& ticket_;
};
//////////////////////////////////////////////////////////////////////////

CMsnService::CMsnService(void)
: acl::ipc_service(1, true)
{
}

CMsnService::~CMsnService(void)
{
}

static void finish_sso(MSN_IPC_DAT* data)
{
	sso_request* req_sso = data->req_sso;
	acl_assert(req_sso);
	CMsnClient* client = req_sso->GetClient();
	acl_assert(client);

	CMsnTicket* ticket = req_sso->GetTicket();
	client->OnSSOFinish(ticket);

	// 删除动态对象
	delete req_sso;
}

static void finish_contact(MSN_IPC_DAT* data, bool ok)
{
	contact_request* req_contact = data->req_contact;
	acl_assert(req_contact);
	CMsnNSClient* ns_client = req_contact->GetNsClient();
	acl_assert(ns_client);

	ns_client->OnGetContactFinish(ok);
	delete req_contact;
}

void CMsnService::win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg < WM_USER)
		return;

	MSN_IPC_DAT* data= (MSN_IPC_DAT*) lParam;

	switch (msg)
	{
	case MSN_TICKET:
	case MSN_TICKET_ERR:
		finish_sso(data);
		acl_myfree(data);
		break;
	case MSN_CONTACT:
		finish_contact(data, true);
		acl_myfree(data);
		break;
	case MSN_CONTACT_ERR:
		finish_contact(data, false);
		acl_myfree(data);
		break;
	default:
		break;
	}
}

void CMsnService::start_sso(CMsnClient* client,
	const char* account, const char* passwd)
{
	// 创建子线程的请求对象
	sso_request* ipc_req = NEW sso_request(client, account, passwd);

	// 调用基类 ipc_service 请求过程
	request(ipc_req);
}

void CMsnService::start_get_contact(CMsnNSClient* ns_client,
	const CMsnTicket& ticket)
{
	// 创建子线程的请求对象
	contact_request* ipc_req = NEW contact_request(ns_client, ticket);

	// 调用基类 ipc_service 请求过程
	request(ipc_req);
}