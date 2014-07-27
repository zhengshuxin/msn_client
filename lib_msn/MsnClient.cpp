#include "stdafx.h"
#include "MsnDSClient.h"
#include "MsnNSClient.h"
#include "MsnSSO.h"
#include "MsnTicket.h"
#include "MsnService.h"
#include "MsnClient.h"

CMsnClient::CMsnClient(IMsnClient* callback, const char* account,
	const char* passwd, int timeout)
: callback_(callback)
, account_(account)
, passwd_(passwd)
, ds_client_(NULL)
, ns_client_(NULL)
, ticket_(NULL)
, timeout_(timeout)
, sid_(1)
{
	msn_service_ = NEW CMsnService();
	acl_assert(callback);
	acl::log::open("./msg_client.log", "test");
}

CMsnClient::~CMsnClient(void)
{
	if (ticket_)
		delete ticket_;
	delete msn_service_;

	if (ds_client_)
		delete ds_client_;
	if (ns_client_)
		delete ns_client_;
}

void CMsnClient::Login(const char* domain, unsigned short port)
{
	ds_client_ = NEW CMsnDSClient(account_.c_str(), this);
	ds_client_->Login(domain, port, timeout_);
}

void CMsnClient::OnDSOk(const acl::string& ns_addr)
{
	acl_assert(ds_client_);
	ns_addr_ = ns_addr;
	delete ds_client_;
	ds_client_ = NULL;

	acl::string addr(ns_addr_.c_str());
	char* ptr = strchr(addr.c_str(), ':');
	if (ptr == NULL || *(ptr + 1) == 0)
	{
		logger_error("invalid ns_addr(%s)", ns_addr.c_str());
		return;
	}
	*ptr++ = 0;
	unsigned short port = atoi(ptr);
	if (port == 0)
	{
		logger_error("invalid ns_port(0)");
		return;
	}

	ns_client_ = NEW CMsnNSClient(account_.c_str(), this);
	ns_client_->Open(addr.c_str(), port, timeout_);
}

void CMsnClient::OnDSErr()
{
	acl_assert(ds_client_);
	delete ds_client_;
	ds_client_ = NULL;
}

void CMsnClient::OnNSUsrSSO()
{
	acl_assert(ns_client_);

	// 异步发起 SSO 请求过程
	msn_service_->start_sso(this, account_.c_str(), passwd_.c_str());
}

void CMsnClient::OnSSOFinish(CMsnTicket* ticket)
{
	ticket_ = ticket;
	if (ticket_ == NULL)
	{
		ns_client_->Close();
		printf(">>get ticket error\r\n");
		return;
	}
	printf(">>get ticket ok\r\n");
	ns_client_->Login(ticket_);
}

void CMsnClient::OnNSErr()
{
	acl_assert(ns_client_);
	ns_client_->Close();
}

void CMsnClient::OnNSClose()
{
	if (ns_client_)
	{
		//delete ns_client_;
		ns_client_ = NULL;
	}
}

int CMsnClient::Sid()
{
	return (sid_++);
}
