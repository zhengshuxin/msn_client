#include "stdafx.h"
#include <iostream>
#include "MsnGlobal.h"
#include "MsnChat.h"

// IPC 方式进行DNS查询的类定义
class dns_result : public acl::dns_result_callback
{
public:
	dns_result(const char* domain, CMsnChat* chat)
		: dns_result_callback(domain)
		, chat_(chat)
	{

	}
	~dns_result() {}

	virtual void destroy()
	{
		delete this;
	}

	virtual void on_result(const char* domain,  const acl::dns_res& res)
	{
		//std::cout << "result: domain: " << domain;
		if (res.ips_.size() == 0)
		{
			std::cout << ": null" << std::endl;
			return;
		}
		//std::cout << std::endl;

		std::list<acl::string>::const_iterator cit = res.ips_.begin();
		for (; cit != res.ips_.end(); cit++)
		{
			//std::cout << "\t" << (*cit).c_str();
			chat_->OnDnsResult((*cit).c_str());
			break;
		}
		//std::cout << std::endl;
	}
private:
	CMsnChat* chat_;
};

CMsnChat::CMsnChat(const char* class_name /* = "unknown" */)
: stream_(NULL)
, debug_(false)
, debug_fpout_(NULL)
, opened_(false)
{
	if (class_name)
		class_name_ = class_name;
	else
		class_name_ = "unkown";
}

CMsnChat::~CMsnChat(void)
{
	//if (stream_)
	//	stream_->close();
	if (debug_fpout_)
		delete debug_fpout_;
	printf(">>CMsnChat: %s delete now\r\n", class_name_.c_str());
}

void CMsnChat::Debug(bool onoff)
{
	debug_ = onoff;
	if (debug_ && debug_fpout_ == NULL)
	{
		debug_fpout_ = NEW acl::ofstream();
		if (debug_fpout_->open_write("debug.txt") == false)
		{
			logger_error("open debug.txt error");
			delete debug_fpout_;
			debug_fpout_ = NULL;
		}
	}
}

void CMsnChat::logger_format(const char* fmt, ...)
{
	if (debug_fpout_ == NULL)
		return;

	va_list ap;
	va_start(ap, fmt);
	debug_fpout_->vformat(fmt, ap);
	va_end(ap);
}

void CMsnChat::OnDnsResult(const char* ip)
{
	acl::string addr(ip);
	addr << ":" << port_;
	Open(addr.c_str(), conn_timeout_);
}

void CMsnChat::Open(const char* domain, unsigned short port, int timeout)
{
	port_ = port;
	conn_timeout_ = timeout;
	dns_result* dns = NEW dns_result(domain, this);
	acl::dns_service& ds = get_dns_service();
	ds.lookup(dns);
}

bool CMsnChat::Open(const char* addr, int timeout)
{
	acl::aio_handle& handle = get_aio_handle();
	stream_ = acl::aio_socket_stream::open(&handle, addr, timeout);
	if (stream_ == NULL)
	{
		logger_error("open %s error(%s)", addr, acl_last_serror());
		return (false);
	}

	stream_->add_open_callback(this);
	stream_->add_close_callback(this);
	stream_->add_timeout_callback(this);
	return (true);
}

void CMsnChat::Close()
{
	if (stream_)
	{
		stream_->close();
		stream_ = NULL;
	}
}

bool CMsnChat::Send(const char* data, size_t len)
{
	if (stream_ == NULL || opened_ == false)
		return (false);
	if (debug_)
	{
		printf("%s: >>send: (%s)\r\n", __FUNCTION__, data);
		logger_format("Send: %s\r\n", data);
	}
	stream_->write(data, (int) len);
	return (true);
}

bool CMsnChat::Gets(bool nonl /* = true */)
{
	if (stream_ == NULL || opened_ == false)
		return (false);
	stream_->gets(0, nonl);
	return (true);
}

bool CMsnChat::Read(int len /* = 0 */)
{
	if (stream_ == NULL || opened_ == false)
		return (false);
	stream_->read(len);
	return (true);
}

bool CMsnChat::read_callback(char* data, int len)
{
	if (debug_)
	{
		printf("%s>>>read: (%s), len(%d)\r\n", __FUNCTION__, data, len);
		logger_format("Read: %s\r\n", data);
	}
	OnRead(data, len);
	return (true);
}

bool CMsnChat::write_callback()
{
	OnSend();
	return (true);
}

void CMsnChat::close_callback()
{
	opened_ = false;
	OnClose();
}

bool CMsnChat::timeout_callback()
{
	OnTimeout();
	return (true);
}

bool CMsnChat::open_callback()
{
	stream_->add_read_callback(this);
	stream_->add_write_callback(this);
	opened_ = true;
	OnOpen();
	return (true);
}