#include "stdafx.h"
#include ".\msnticket.h"

#define SKIP_WHILE(cond, ptr) { while(*(ptr) && (cond)) (ptr)++; }
#define SKIP_SPACE(ptr) { while(IS_SPACE(*(ptr))) (ptr)++; }

CMsnTicket::CMsnTicket(void)
{

}

CMsnTicket::~CMsnTicket(void)
{
	std::list<TICKET*>::iterator it = tickets_.begin();
	for (; it != tickets_.end(); it++)
	{
		TICKET* ticket = *it;
		if (ticket->id)
			acl_myfree(ticket->id);
		if (ticket->domain)
			acl_myfree(ticket->domain);
		if (ticket->policy)
			acl_myfree(ticket->policy);
		if (ticket->secret)
			acl_myfree(ticket->secret);
		if (ticket->expires)
			acl_myfree(ticket->expires);
		if (ticket->ticket)
			acl_myfree(ticket->ticket);
		if (ticket->p)
			acl_myfree(ticket->p);
		acl_myfree(ticket);
	}
	tickets_.clear();
}

void CMsnTicket::AddTicket(const char* id, const char* domain,
	const char* secret, const char* expires, const char* txt)
{
	ACL_VSTRING* vbuf = acl_vstring_alloc(256);
	acl_html_decode(txt, vbuf);

	const char* t = NULL, *p = NULL;
	ACL_ARGV* tokens = acl_argv_split(acl_vstring_str(vbuf), "&");
	ACL_ITER iter;

	acl_foreach(iter, tokens)
	{
		char* token = (char*) iter.data;
		char* name = token;
		char* value = strchr(name, '=');
		if (value == NULL)
			continue;

		*value++ = 0;

		SKIP_WHILE(*value == '=' || *value == ' ' || *value == '\t', value);
		if (*value == 0)
			continue;
		if (iter.i > 0)
			printf(", ");
		//printf("%s=%s", name, value);
		if (strcasecmp(name, "t") == 0)
			t = value;
		else if (strcasecmp(name, "p") == 0)
			p = value;
	}

	//printf("\r\n");

	if (t == NULL)
	{
		acl_argv_free(tokens);
		acl_vstring_free(vbuf);
		return;
	}

	TICKET* ticket = (TICKET*) acl_mycalloc(1, sizeof(TICKET));
	if (id)
		ticket->id = acl_mystrdup(id);
	if (domain)
		ticket->domain = acl_mystrdup(domain);
	if (secret)
		ticket->secret = acl_mystrdup(secret);
	if (expires)
		ticket->expires = acl_mystrdup(expires);

	ticket->ticket = acl_mystrdup(t);
	if (p)
		ticket->p = acl_mystrdup(p);
	tickets_.push_back(ticket);

	acl_argv_free(tokens);
	acl_vstring_free(vbuf);
}

void CMsnTicket::SetCipher(const char* cipher)
{
	if (cipher)
		cipher_ = cipher;
}

void CMsnTicket::SetSecret(const char* secret)
{
	if (secret)
		secret_ = secret;
}

const TICKET* CMsnTicket::GetTicket(const char* domain) const
{
	const TICKET* ticket = NULL;
	std::list<TICKET*>::const_iterator cit = tickets_.begin();
	for (; cit != tickets_.end(); cit++)
	{
		if (strcasecmp((*cit)->domain, domain) == 0)
		{
			ticket = *cit;
			break;
		}
	}

	return (ticket);
}