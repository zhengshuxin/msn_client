#pragma once
#include "lib_msn.h"

//////////////////////////////////////////////////////////////////////////
// msnclient 的接口

class LIB_MSN_API IMsnClient
{
public:
	IMsnClient() {}
	virtual ~IMsnClient() {}

	/**
	* 登入成功时的回调函数
	*/
	virtual void OnLoginOk() {}

	/**
	* 登入失败时的回调函数
	*/
	virtual void OnLoginErr() {}
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

class CMsnDSClient;
class CMsnNSClient;
class CMsnTicket;
class CMsnStatus;
class CMsnService;

// MSN 客户端库
class LIB_MSN_API CMsnClient
{
public:
	/**
	* 构造函数
	* @param callback {IMsnClient*} MSN 协议处理过程中的回调函数
	* @param account {const char*} 登入帐号
	* @param passwd {const char*} 密码
	* @param timeout {int} 超时时间值
	*/
	CMsnClient(IMsnClient* callback, const char* account,
		const char* passwd, int timeout);
	~CMsnClient(void);

	/**
	* 开始 MSN 登入过程
	* @param domain {const char*} 登入的服务器域名
	* @param port {unsigned short} 服务器端口
	*
	*/
	void Login(const char* domain, unsigned short port);

	void OnDSOk(const acl::string& ns_addr);
	void OnDSErr();

	void OnNSUsrSSO();
	void OnNSErr();
	void OnNSClose();
	int  Sid();

	IMsnClient& GetCallback() const
	{
		return (*callback_);
	}

	// 获得异步过程的服务句柄
	CMsnService& GetService()
	{
		return (*msn_service_);
	}

	CMsnNSClient* GetNSClient()
	{
		return (ns_client_);
	}

	/**
	* SSO 登入成功或失败的回调函数
	* @param ticket 如果失败，则该参数为空
	*/
	void OnSSOFinish(CMsnTicket* ticket);
protected:
private:
	IMsnClient* callback_;
	acl::string account_;
	acl::string passwd_;
	CMsnDSClient* ds_client_;
	CMsnNSClient* ns_client_;
	CMsnTicket* ticket_;
	acl::string ns_addr_;
	int   timeout_;
	int   sid_;

	CMsnService* msn_service_;
};
