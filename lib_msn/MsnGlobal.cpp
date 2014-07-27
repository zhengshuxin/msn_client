#include "stdafx.h"
#include "MsnGlobal.h"

static void free_handle(acl::aio_handle* handle)
{
	delete handle;
}

acl::aio_handle& get_aio_handle(bool use_gui /* = false */)
{
	static acl_pthread_key_t handle_key = -1;
	acl::aio_handle* handle = (acl::aio_handle*) acl_pthread_tls_get(&handle_key);
	if (handle == NULL)
	{
		if (use_gui)
			handle = NEW acl::aio_handle(acl::ENGINE_WINMSG);
		else
			handle = NEW acl::aio_handle(acl::ENGINE_SELECT);
		acl_pthread_tls_set(handle_key, handle, (void (*)(void*)) free_handle);
	}
	return (*handle);
}

static void free_dns(acl::dns_service* dns)
{
	//delete dns;
}

acl::dns_service& get_dns_service()
{
	static acl_pthread_key_t dns_key = -1;
	acl::dns_service* dns = (acl::dns_service*) acl_pthread_tls_get(&dns_key);
	if (dns == NULL)
	{
		dns = NEW acl::dns_service(5);
		acl_pthread_tls_set(dns_key, dns, (void (*)(void*)) free_dns);
		acl::aio_handle& handle = get_aio_handle(true);
		if (dns->open(&handle) == false)
			logger_fatal("open dns service error(%s)", acl_last_serror());
	}
	return (*dns);
}