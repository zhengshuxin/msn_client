#include "stdafx.h"
#include "Util.h"
#include "MsnOim.h"

/*
MSG Hotmail Hotmail 289
MIME-Version: 1.0
Content-Type: text/x-msmsgsinitialmdatanotification; charset=UTF-8

Mail-Data: <MD><E><I>0</I><IU>0</IU><O>0</O><OU>0</OU></E><Q><QTM>409600</QTM><QNM>204800</QNM></Q></MD>
Inbox-URL: /cgi-bin/HoTMaiL
Folders-URL: /cgi-bin/folders
Post-URL: http://www.hotmail.com
*/

CMsnOim::CMsnOim(const acl::mime& mime)
{
	const char* pMailData = mime.header_value("Mail-Data");
	if (strcasecmp(pMailData, "too-large") == 0)
		acl_assert(0);

	acl::xml xml;
	xml.update(pMailData);

	inbox_total_ = GetFirstInt(xml, "MD/E/I");
	inbox_unread_ = GetFirstInt(xml, "MD/E/IU");
	other_total_ = GetFirstInt(xml, "MD/E/O");
	other_unread_ = GetFirstInt(xml, "MD/E/OU");
	QTM = GetFirstInt(xml, "MD/Q/QTM");
	QNM = GetFirstInt(xml, "MD/Q/QNM");

	const char* ptr = mime.header_value("Inbox-URL");
	if (ptr)
		inbox_url_ = acl_mystrdup(ptr);
	else
		inbox_url_ = NULL;

	ptr = mime.header_value("Folders-URL");
	if (ptr)
		folders_url_ = acl_mystrdup(ptr);
	else
		folders_url_ = NULL;

	ptr = mime.header_value("Post-URL");
	if (ptr)
		post_url_ = acl_mystrdup(ptr);
	else
		post_url_ = NULL;
}

CMsnOim::~CMsnOim()
{
#define SAFE_FREE(x) { if ((x)) acl_myfree((x)); }

	SAFE_FREE(inbox_url_)
	SAFE_FREE(folders_url_)
	SAFE_FREE(post_url_)
}

/*
* <MD>
*     <E>
*         <I>884</I>     Inbox total
*         <IU>0</IU>     Inbox unread mail
*         <O>222</O>     Sent + Junk + Drafts
*         <OU>15</OU>    Junk unread mail
*     </E>
*     <Q>
*         <QTM>409600</QTM>
*         <QNM>204800</QNM>
*     </Q>
*     <M>
*         <!-- OIM Nodes -->
*     </M>
*     <M>
*         <!-- OIM Nodes -->
*     </M>
* </MD>
*/
CMsnOim* CMsnOim::Create(const acl::mime& mime)
{
	const char* ptr = mime.header_value("Mail-Data");
	if (strcasecmp(ptr, "too-large") == 0)
		return (NULL);

	CMsnOim* oim = NEW CMsnOim(mime);
	return (oim);
}