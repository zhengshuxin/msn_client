#include "stdafx.h"
#include "lib_acl.h"
#include "lib_acl.hpp"
#include "lib_protocol.h"
#include "MsnSSOTMP.h"
#include "MsnTicket.h"
#include ".\msnsso.h"

struct TICKET_DOMAIN
{
	const char* domain;
	const char* policy;
	const char* compact;
};

static const TICKET_DOMAIN ticket_domains[] =
{
	/* http://msnpiki.msnfanatic.com/index.php/MSNP15:SSO */
	/* {"Domain", "Policy Ref URI", "Compact" }, Purpose */
	{"messengerclear.live.com", NULL, "Compact1" },       /* Authentication for messenger. */
	{"messenger.msn.com", "?id=507", "Compact2" },        /* Authentication for receiving OIMs. */
	{"contacts.msn.com", "MBI", "Compact3" },             /* Authentication for the Contact server. */
	{"messengersecure.live.com", "MBI_SSL", "Compact4" }, /* Authentication for sending OIMs. */
	{"spaces.live.com", "MBI", "Compact5" },              /* Authentication for the Windows Live Spaces */
	{"livecontacts.live.com", "MBI", "Compact6" },        /* Live Contacts API, a simplified version of the Contacts SOAP service */
	{"storage.live.com", "MBI", "Compact7" },             /* Storage REST API */
};

CMsnSSO::CMsnSSO(const char* username, const char* password, const char* sso_policy)
: m_username(username)
{
	acl_xml_encode(password, m_password.vstring());
	if (sso_policy)
		m_ssoPolicy = sso_policy;
	else
		m_ssoPolicy = "MBI_KEY";

	debug_fpout_ = NEW acl::ofstream();
	if (debug_fpout_->open_write("sso.txt") == false)
	{
		logger_error("open sso.txt error");
		delete debug_fpout_;
		debug_fpout_ = NULL;
	}
}

CMsnSSO::~CMsnSSO(void)
{
	if (debug_fpout_)
		delete debug_fpout_;
}

void CMsnSSO::logger_format(const char* fmt, ...)
{
	if (debug_fpout_ == NULL)
		return;

	va_list ap;
	va_start(ap, fmt);
	debug_fpout_->vformat(fmt, ap);
	va_end(ap);
}

void CMsnSSO::BuildRequest(acl::string& request)
{
	ticket_domain_count_ = sizeof(ticket_domains) / sizeof(ticket_domains[0]);
	acl::string domains;

	for (size_t i = 0; i < ticket_domain_count_; i++)
	{
		domains.format_append(MSN_SSO_RST_TEMPLATE, i + 1,
			ticket_domains[i].domain,
			ticket_domains[i].policy != NULL ?
				ticket_domains[i].policy : m_ssoPolicy.c_str());
	}
	request.format(MSN_SSO_TEMPLATE, m_username.c_str(),
		m_password.c_str(), domains.c_str());
}

CMsnTicket* CMsnSSO::GetTicket()
{
	acl::string addr(MSN_SSO_SERVER);
	addr << ":443";

	// 连接 MSN SSO 服务器(ssl 方式)

	acl::http_client client;
	if (client.open(addr.c_str()) == false)
		return (NULL);

	//////////////////////////////////////////////////////////////////////////
	// 向 MSN SSO 服务器发送请求数据

	acl::string request_body;
	BuildRequest(request_body);

	// 创建 HTTP 请求头
	acl::http_header header;
	header.set_method(acl::HTTP_METHOD_POST);
	header.set_url(SSO_POST_URL);
	header.set_host(MSN_SSO_SERVER);
	header.set_keep_alive(false);
	header.set_content_length(request_body.length());
	header.set_content_type("text/xml; charset=gb2312");
	header.add_entry("SOAPAction", "");
	header.add_entry("Cache-Control", "no-cache");
	header.add_entry("Accept", "*/*");
	header.add_entry("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1");
	header.accept_gzip(true);

	// 发送 HTTP 请求头

	if (client.write(header) == -1)
	{
		logger_error("write http header error");
		return (NULL);
	}

	// 记日志
	acl::string header_buf;
	header.build_request(header_buf);
	logger_format("send: %s\r\n", header_buf.c_str());

	//printf("---------------------------------------------\r\n");
	//printf("%s", request.c_str());
	//printf("---------------------------------------------\r\n");

	// 发送 HTTP 请求体

	// 记日志
	logger_format("send: %s\r\n", request_body.c_str());

	if (client.get_ostream().write(request_body) == -1)
	{
		logger_error("write http body error");
		return (NULL);
	}

	//////////////////////////////////////////////////////////////////////////
	// 从 MSN SSO 服务器读取响应数据

	// 读取响应头
	if (client.read_head() == false)
	{
		logger_error("read http respond head error");
		return (NULL);
	}

	// 获得响应头对象

	const HTTP_HDR_RES* hdr_res = client.get_respond_head(NULL);

	// 记日志，记下响应头
	ACL_VSTREAM* out;
	if (debug_fpout_)
		out = debug_fpout_->get_vstream();
	else
		out = NULL;
	if (debug_fpout_)
		http_hdr_fprint(out, &hdr_res->hdr, "read header: ");

	if (hdr_res->hdr.content_length == 0
		|| (hdr_res->hdr.content_length == -1
		&& !hdr_res->hdr.chunked
		&& hdr_res->reply_status > 300
		&& hdr_res->reply_status < 400))
	{
		logger_error("http respond no body");
		return (NULL);
	}

	/* 读书 HTTP 响应体 */

	acl::string buf;
	ACL_XML* body = acl_xml_alloc();
	int   ret;
	while(true)
	{
		ret = client.read_body(buf);
		if (ret < 0)
			break;
		else if (client.body_finish())
			break;
		acl_xml_update(body, buf.c_str());
		logger_format("read: %s\r\n", buf.c_str());
	}

	//////////////////////////////////////////////////////////////////////////

	const char* tags = "S:Body/wst:RequestSecurityTokenResponseCollection";
	ACL_ARRAY* collections = acl_xml_getElementsByTags(body, tags);

	if (collections == NULL)
	{
		acl_xml_free(body);
		return (NULL);
	}

	CMsnTicket* ticket = NEW CMsnTicket();

	if (ParseCollection(ticket, collections) == false)
	{
		delete ticket;
		acl_xml_free(body);
		return (NULL);
	}

	acl_xml_free(body);
	return (ticket);
}

#define STR	acl_vstring_str
#define LEN	ACL_VSTRING_LEN

bool CMsnSSO::ParsePassport(CMsnTicket* ticket, ACL_XML_NODE* node, const char* addr)
{
	ACL_XML xml;
	acl_xml_foreach_init(&xml, node);

	const char* tags_cipher = "wst:RequestedSecurityToken/EncryptedData/CipherData/CipherValue";
	ACL_ARRAY* nodes_cipher = acl_xml_getElementsByTags(&xml, tags_cipher);
	if (nodes_cipher == NULL)
	{
		logger_error("tag(%s) not find", tags_cipher);
		return (false);
	}
	ACL_XML_NODE* cipher = (ACL_XML_NODE*) nodes_cipher->items[0];
	if (cipher->text == NULL)
	{
		logger_error("tag(%s) null", tags_cipher);
		return (false);
	}
	ticket->SetCipher(STR(cipher->text));
	acl_xml_free_array(nodes_cipher);

	const char* tags_secret = "wst:RequestedProofToken/wst:BinarySecret";
	ACL_ARRAY* nodes_secret = acl_xml_getElementsByTags(&xml, tags_secret);
	if (nodes_secret == NULL)
	{
		logger_error("tag(%s) not found", tags_secret);
		return (false);
	}
	ACL_XML_NODE* secret = (ACL_XML_NODE*) nodes_secret->items[0];
	if (secret->text == NULL)
	{
		logger_error("tag(%s) null", tags_secret);
		acl_xml_free_array(nodes_secret);
		return (false);
	}
	ticket->SetSecret(STR(secret->text));

	acl_xml_free_array(nodes_secret);
	return (true);
}

bool CMsnSSO::ParseDomain(CMsnTicket* ticket, ACL_XML_NODE* node, const char* addr)
{
	ACL_XML xml;
	ACL_XML_NODE* token;
	acl_xml_foreach_init(&xml, node);

	const char* tags_token = "wst:RequestedSecurityToken/wsse:BinarySecurityToken";
	ACL_ARRAY* a_token = acl_xml_getElementsByTags(&xml, tags_token);
	if (a_token == NULL)
	{
		logger_error("tag(%s) not found", tags_token);
		return (false);
	}
	token = (ACL_XML_NODE*) a_token->items[0];
	if (token->text == NULL || token->id == NULL)
	{
		logger_error("tag(%s) invalid", tags_token);
		acl_xml_free_array(a_token);
		return (false);
	}
	const char* txt = STR(token->text);
	const char* id = STR(token->id);

	const char* tags_secret = "wst:RequestedProofToken/wst:BinarySecret";
	ACL_ARRAY* a_secret = acl_xml_getElementsByTags(&xml, tags_secret);
	const char* secret = NULL;
	if (a_secret)
	{
		token = (ACL_XML_NODE*) a_secret->items[0];
		if (token->text && LEN(token->text) > 0)
			secret = STR(token->text);
	}

	const char* tags_expires = "wst:LifeTime/wsu:Expires";
	ACL_ARRAY* a_expires = acl_xml_getElementsByTags(&xml, tags_expires);
	const char* expires = NULL;
	if (a_expires)
	{
		token = (ACL_XML_NODE*) a_expires->items[0];
		if (token->text && LEN(token->text) > 0)
			expires = STR(token->text);
	}

	ticket->AddTicket(id, addr, secret, expires, txt);

	if (a_expires)
		acl_xml_free_array(a_expires);
	if (a_secret)
		acl_xml_free_array(a_secret);
	acl_xml_free_array(a_token);
	return (true);
}

bool CMsnSSO::ParseResponse(CMsnTicket* ticket, ACL_XML_NODE* node)
{
	ACL_XML xml;
	acl_xml_foreach_init(&xml, node);
	ACL_ARRAY* a = acl_xml_getElementsByTags(&xml, "wst:RequestSecurityTokenResponse");
	if (a == NULL)
		return (false);

	ACL_ITER iter;
	const char* tag_addr = "wsp:AppliesTo/wsa:EndpointReference/wsa:Address";

	acl_foreach(iter, a)
	{
		node = (ACL_XML_NODE*) iter.data;
		acl_xml_foreach_init(&xml, node);
		ACL_ARRAY* addrs = acl_xml_getElementsByTags(&xml, tag_addr);
		if (addrs == NULL)
			continue;

		ACL_XML_NODE* first_addr = (ACL_XML_NODE*) addrs->items[0];
		if (first_addr->text == NULL)
		{
			acl_xml_free_array(addrs);
			continue;
		}

		char* addr = STR(first_addr->text);
		if (acl_strcasestr(addr, "http://Passport.NET/tb") != 0)
			ParsePassport(ticket, node, addr);
		else
			ParseDomain(ticket, node, addr);
		acl_xml_free_array(addrs);
	}

	acl_xml_free_array(a);
	return (true);
}

bool CMsnSSO::ParseCollection(CMsnTicket* ticket, ACL_ARRAY* collections)
{
	ACL_ITER iter;

	acl_foreach(iter, collections)
	{
		ACL_XML_NODE* node = (ACL_XML_NODE*) iter.data;
		if (ParseResponse(ticket, node) == false)
			return (false);
	}

	return (true);
}