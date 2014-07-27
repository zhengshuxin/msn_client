#include "stdafx.h"
#include "lib_acl.h"
#include "lib_acl.hpp"
#include "Util.h"

char* var_emty_string = "";

char* GetFirstText(ACL_XML* xml, const char* tag, bool* no_exist /* NULL */)
{
	ACL_ARRAY* a = acl_xml_getElementsByTags(xml, tag);
	if (a == NULL)
	{
		//logger("tag(%s) not found", tag);
		if (no_exist)
			*no_exist = true;
		return (var_emty_string);
	}

	if (no_exist)
		*no_exist = false;

	ACL_XML_NODE* first = (ACL_XML_NODE*) a->items[0];
	if (first->text == NULL)
	{
		//logger("tag(%s)'s text null", tag);
		acl_xml_free_array(a);
		return (var_emty_string);
	}

	char* out = acl_mystrdup(acl_vstring_str(first->text));
	acl_xml_free_array(a);
	return (out);
}

bool  GetFirstBool(ACL_XML* xml, const char* tag, bool* no_exist /* NULL */)
{
	char* ptr = GetFirstText(xml, tag, no_exist);

	if (STRING_IS_EMPTY(ptr))
		return (false);

	bool ret;
	if (strcasecmp(ptr, "true") == 0)
		ret = true;
	else
		ret = false;

	STRING_SAFE_FREE(ptr);
	return (ret);
}

int GetFirstInt(ACL_XML* xml, const char* tag, bool* no_exist /* NULL */)
{
	char* ptr = GetFirstText(xml, tag, no_exist);

	if (STRING_IS_EMPTY(ptr))
		return (0);

	int n = atoi(ptr);
	STRING_SAFE_FREE(ptr);
	return (n);
}

//////////////////////////////////////////////////////////////////////////

char* GetFirstText(acl::xml& xml, const char* tag, bool* no_exist /* NULL */)
{
	const std::vector<acl::xml_node*>& nodes = xml.getElementsByTags(tag);
	if (nodes.empty())
	{
		//logger("tag(%s) not found", tag);
		if (no_exist)
			*no_exist = true;
		return (var_emty_string);
	}

	if (no_exist)
		*no_exist = false;

	acl::xml_node* first = nodes[0];
	const char* text = first->text();
	if (text == NULL)
	{
		//logger("tag(%s)'s text null", tag);
		return (var_emty_string);
	}

	char* out = acl_mystrdup(text);
	return (out);
}

bool  GetFirstBool(acl::xml& xml, const char* tag, bool* no_exist /* NULL */)
{
	char* ptr = GetFirstText(xml, tag, no_exist);

	if (STRING_IS_EMPTY(ptr))
		return (false);

	bool ret;
	if (strcasecmp(ptr, "true") == 0)
		ret = true;
	else
		ret = false;

	STRING_SAFE_FREE(ptr);
	return (ret);
}

int GetFirstInt(acl::xml& xml, const char* tag, bool* no_exist /* NULL */)
{
	char* ptr = GetFirstText(xml, tag, no_exist);

	if (STRING_IS_EMPTY(ptr))
		return (0);

	int n = atoi(ptr);
	STRING_SAFE_FREE(ptr);
	return (n);
}

//////////////////////////////////////////////////////////////////////////

bool MsnEmailIsValid(const char *passport)
{
	if (!EmailIsValid(passport))
		return false;

	/* Special characters aren't allowed in domains, so only go to '@' */
	while (*passport != '@') {
		if (*passport == '/')
			return false;
		else if (*passport == '?')
			return false;
		else if (*passport == '=')
			return false;
		/* MSN also doesn't like colons, but that's checked already */

		passport++;
	}
	return true;
}

bool EmailIsValid(const char *address)
{
	const char *c, *domain;
	static char *rfc822_specials = "()<>@,;:\\\"[]";

	if (*address == '.')
		return false;

	/* first we validate the name portion (name@domain) (rfc822)*/
	for (c = address;  *c;  c++)
	{
		if (*c == '\"' && (c == address || *(c - 1) == '.' || *(c - 1) == '\"'))
		{
			while (*++c)
			{
				if (*c == '\\')
				{
					if (*c++ && *c < 127 && *c != '\n' && *c != '\r')
						continue;
					else
						return false;
				}
				if (*c == '\"')
					break;
				if (*c < ' ' || *c >= 127)
					return false;
			}
			if (!*c++)
				return false;
			if (*c == '@')
				break;
			if (*c != '.')
				return false;
			continue;
		}
		if (*c == '@')
			break;
		if (*c <= ' ' || *c >= 127) return
			false;
		if (strchr(rfc822_specials, *c))
			return false;
	}

	/* It's obviously not an email address if we didn't find an '@' above */
	if (*c == '\0')
		return false;

	/* strictly we should return false if (*(c - 1) == '.') too, but I think
	* we should permit user.@domain type addresses - they do work :) */
	if (c == address)
		return false;

	/* next we validate the domain portion (name@domain) (rfc1035 & rfc1011) */
	if (!*(domain = ++c))
		return false;
	do {
		if (*c == '.' && (c == domain || *(c - 1) == '.' || *(c - 1) == '-'))
			return false;
		if (*c == '-' && (*(c - 1) == '.' || *(c - 1) == '@'))
			return false;
		if ((*c < '0' && *c != '-' && *c != '.') || (*c > '9' && *c < 'A') ||
			(*c > 'Z' && *c < 'a') || (*c > 'z'))
		{
			return false;
		}
	} while (*++c);

	if (*(c - 1) == '-')
		return false;

	return ((c - domain) > 3 ? true : false);
}

void RandGuid(char* buf, size_t size)
{
	_snprintf(buf, size, "%4X%4X-%4X-%4X-%4X-%4X%4X%4X",
		rand() % 0xAAFF + 0x1111,
		rand() % 0xAAFF + 0x1111,
		rand() % 0xAAFF + 0x1111,
		rand() % 0xAAFF + 0x1111,
		rand() % 0xAAFF + 0x1111,
		rand() % 0xAAFF + 0x1111,
		rand() % 0xAAFF + 0x1111,
		rand() % 0xAAFF + 0x1111);
}
