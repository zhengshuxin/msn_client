#include "stdafx.h"

#include "MsnGlobal.h"
#include "MsnClient.h"
#include ".\msndsclient.h"

CMsnDSClient::CMsnDSClient(const char* account, CMsnClient* client)
: CMsnChat("CMsnDSClient")
, account_(account)
, client_(client)
, xfr_ver_(0)
, xfr_cnt_(0)
{
}

CMsnDSClient::~CMsnDSClient(void)
{
}

void CMsnDSClient::Login(const char* domain, unsigned short port, int timeout)
{
	Open(domain, port, timeout);
}

void CMsnDSClient::OnOpen()
{
	Debug(true);

	acl::string msg("VER ");
	msg << client_->Sid();
#if defined(MSN_PROTOCOL15)
	msg << " MSNP15 CVR0\r\n";
#elif defined(MSN_PROTOCOL21)
	msg << " MSNP21 CVR0\r\n";
#else
#error "unknow MSNP"
#endif
	Send(msg.c_str(), msg.length());

	msg = "CVR ";
	msg << client_->Sid() << " 0x0804 winnt 6.1.1 i386 MSNMSGR 15.4.3508.1109 MSNMSGR ";
	msg << account_.c_str();
#if defined(MSN_PROTOCOL15)
	msg << "\r\n";
#elif defined(MSN_PROTOCOL21)
	msg << " 0\r\n";
#else
#error "unknown MSNP"
#endif

	Send(msg.c_str(), msg.length());

	msg = "USR ";
	msg << client_->Sid() << " SSO I ";
	msg << account_.c_str() << "\r\n";
	Send(msg.c_str(), msg.length());
	Gets(true);
}

void CMsnDSClient::OnRead(char* data, int len)
{
	char cmd[32], *ptr;

	ptr = strchr(data, ' ');
	if (ptr == NULL || *(ptr + 1) == 0)
	{
		logger_error("invalid data(%s)", data);
		return;
	}
	*ptr++ = 0;
	ACL_SAFE_STRNCPY(cmd, data, sizeof(cmd));

	ACL_ARGV* args = acl_argv_split(ptr, " \t");

	if (strcasecmp(cmd, "VER") == 0)
	{
		// VER 1 MSNP21
		if (args->argc < 2)
			logger_warn("unkown args: %s", ptr);
		else
		{
			curr_seq_ = atoi(args->argv[0]);
			svrproto_ = args->argv[1];
		}
	}
	else if (strcasecmp(cmd, "CVR") == 0)
	{
		// CVR 2 14.0.8117 14.0.8117 14.0.8117 http://msgruser.dlservice.microsoft.com/download/A/6/1/A616CCD4-B0CA-4A3D-B975-3EDB38081B38/fr/wlsetup-cvr.exe http://download.live.com/?sku=messenger
		if (args->argc < 1)
			logger_warn("unkown args: %s", ptr);
		else
			curr_seq_ = atoi(args->argv[0]);
	}
	else if (strcasecmp(cmd, "XFR") == 0)
	{

#if defined(MSN_PROTOCOL15)
		// XFR 3 NS 64.4.34.193:1863 U D
		if (args->argc < 5 || strcasecmp(args->argv[1], "NS") != 0)
			logger_warn("unkown args: %s", ptr);
		else
		{
			curr_seq_ = atoi(args->argv[0]);
			ns_addr_ = args->argv[2];
			xfr_args3_ = args->argv[3][0];
			xfr_args4_ = args->argv[4][0];
		}
#elif defined (MSN_PROTOCOL21)
		// XFR 3 NS 64.4.34.193:1863 U D VmVyc2lvbjogMQ0KWGZyQ291bnQ6IDENCg==
		if (args->argc < 6 || strcasecmp(args->argv[1], "NS") != 0)
			logger_warn("unkown args: %s", ptr);
		else
		{
			curr_seq_ = atoi(args->argv[0]);
			ns_addr_ = args->argv[2];
			xfr_args3_ = args->argv[3][0];
			xfr_args4_ = args->argv[4][0];
			parse_xfr_ver(args->argv[5]);
		}
#endif

		const char out[] = "Out\r\n";
		Send(out, sizeof(out) - 1);
		Close();
	}
	else
		logger_warn("unkown cmd(%s), args(%s)", cmd, ptr);

	acl_argv_free(args);
}

#ifdef MSN_PROTOCOL21
// VmVyc2lvbjogMQ0KWGZyQ291bnQ6IDENCg==
// decode result:Version: 1
// XfrCount: 1
void CMsnDSClient::parse_xfr_ver(const char* data)
{
	acl::string buf;
	buf.base64_decode(data, strlen(data));
	if (buf.empty())
		return;
	const std::list<acl::string>& args = buf.split("\r\n");
	std::list<acl::string>::const_iterator cit = args.begin();
	for (; cit != args.end(); cit++)
	{
		acl::string line = *cit;
		char* name = line.c_str();
		char* value = strchr(name, ':');
		if (value == NULL || *(value + 1) == 0)
		{
			logger_warn("invalid line(%s)", name);
			continue;
		}
		*value++ = 0;
		while (*value == ' ' || *value == '\t')
			value++;
		if (*value == 0)
		{
			logger_warn("invalid line(%s: )", name);
			continue;
		}
		if (strcasecmp(name, "Version") == 0)
			xfr_ver_ = atoi(value);
		else if (strcasecmp(name, "XfrCount") == 0)
			xfr_cnt_ = atoi(value);
	}
}
#endif

void CMsnDSClient::OnSend()
{

}

void CMsnDSClient::OnTimeout()
{

}

void CMsnDSClient::OnClose()
{
	if (!ns_addr_.empty())
		client_->OnDSOk(ns_addr_);
	else
		client_->OnDSErr();
}