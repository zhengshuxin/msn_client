#pragma once
#include "MsnChat.h"
#include "lib_msn.h"

class CMsnClient;

class LIB_MSN_API CMsnDSClient : public CMsnChat
{
public:
	CMsnDSClient(const char* account, CMsnClient* client);
	~CMsnDSClient(void);

	void Login(const char* domain, unsigned short port, int timeout);

protected:
	virtual void OnOpen();
	virtual void OnRead(char* data, int len);
	virtual void OnSend();
	virtual void OnTimeout();
	virtual void OnClose();
private:
	acl::string account_;
	acl::string svrproto_;
	acl::string ns_addr_;
	int   xfr_ver_;
	int   xfr_cnt_;
	int   curr_seq_;
	char  xfr_args3_;
	char  xfr_args4_;
	CMsnClient* client_;

#ifdef MSN_PROTOCOL21
	void parse_xfr_ver(const char* data);
#endif
};
