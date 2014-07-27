#include "stdafx.h"
#include "lib_acl.h"
#include "MsnClient.h"
#include "MsnTicket.h"
#include "MsnUsrKey.h"
#include "MsnClientInfo.h"
#include "MsnQRY.h"
#include "MsnOim.h"
#include "MsnContactManager.h"
#include "MsnADLPayload.h"
#include "MsnGlobal.h"
#include "MsnMemberShips.h"
#include "MsnSwitchBoard.h"
#include "MsnService.h"
#include ".\msnnsclient.h"

CMsnNSClient::CMsnNSClient(const char* account, CMsnClient* client,
	const char* display_name /* = NULL */)
: CMsnChat("CMsnNSClient")
, account_(account)
, client_(client)
, status_(NS_STATUS_CMD)
, ticket_(NULL)
, oim_(NULL)
, logged_in_(false)
, adl_fqy_(0)
{
	if (display_name && display_name)
		display_name_ = display_name;
	else
		display_name_ = account;

#if defined(MSN_PROTOCOL15)
	protocol_ver_ = 15;
#elif defined(MSN_PROTOCOL21)
	protocol_ver_ = 21;
#endif
}

CMsnNSClient::~CMsnNSClient(void)
{
	if (oim_)
		delete oim_;

	std::map<acl::string, CMsnSwitchBoard*>::iterator it = sb_list_.begin();
	for (; it != sb_list_.end(); it++)
	{
		it->second->Quit();
		//delete it->second;
	}
}

// 当与 name server 连接建立后的回调函数
void CMsnNSClient::OnOpen()
{
	Debug(true); // only for test

	acl::string msg("VER ");
	msg << client_->Sid();

#if defined (MSN_PROTOCOL15)
	// 格式: VER {id} MSNP15 MSNP16 CVR0
	msg << " MSNP15 MSNP16 CVR0\r\n";
#elif defined (MSN_PROTOCOL21)
	// 格式: VER {id} MSNP21 CVR0
	msg << " MSNP21 CVR0\r\n";
#else
#error "unknow MSNP"
#endif

	Send(msg.c_str(), msg.length());
	Gets(true);
}

// 读到来自于 NS 服务器的数据时的统一入口
void CMsnNSClient::OnRead(char* data, int len)
{
	// 如果是普通命令字
	if (status_ == NS_STATUS_CMD)
		OnCmd(data, len);

	// 如果是 GCF 命令字的数据
	else if (status_ == NS_STATUS_GCF)
		OnGcfData(data, len);

	// 如果是 MSG 命令字的数据
	else if (status_ == NS_STATUS_MSG)
		OnMsgData(data, len);

	// 如果是 UBX 命令字的数据
	else if (status_ == NS_STATUS_UBX)
		OnUbxData(data, len);
}

void CMsnNSClient::OnCmd(char* data, int len)
{
	char cmd[32], *ptr;

	ptr = strchr(data, ' ');
	if (ptr == NULL || *(ptr + 1) == 0)
	{
		logger_error("invalid data(%s)", data);
		client_->OnNSErr();
		return;
	}
	*ptr++ = 0;
	ACL_SAFE_STRNCPY(cmd, data, sizeof(cmd));

	ACL_ARGV* args = acl_argv_split(ptr, " \t");
	
	int   argc = args->argc;

	if (acl_alldig(args->argv[0]))
		curr_seq_ = atoi(args->argv[0]);

	const char** argv = (const char**) args->argv;

	if (strcasecmp(cmd, "VER") == 0)
		OnCmdVER(argc, argv, ptr);
	else if (strcasecmp(cmd, "CVR") == 0)
		OnCmdCVR(argc, argv, ptr);
	else if (strcasecmp(cmd, "GCF") == 0)
		OnCmdGCF(argc, argv, ptr);
	else if (strcasecmp(cmd, "USR") == 0)
		OnCmdUSR(argc, argv, ptr);
	else if (strcasecmp(cmd, "CHL") == 0)
		OnCmdCHL(argc, argv, ptr);
	else if (strcasecmp(cmd, "SBS") == 0)
		OnCmdSBS(argc, argv, ptr);
	else if (strcasecmp(cmd, "MSG") == 0)
		OnCmdMSG(argc, argv, ptr);
	else if (strcasecmp(cmd, "UBX") == 0)
		OnCmdUBX(argc, argv, ptr);
	else if (strcasecmp(cmd, "QRY") == 0)
		OnCmdQRY(argc, argv, ptr);
	else if (strcasecmp(cmd, "ADL") == 0)
		OnCmdADL(argc, argv, ptr);
	else if (strcasecmp(cmd, "BLP") == 0)
		OnCmdBLP(argc, argv, ptr);
	else if (strcasecmp(cmd, "PRP") == 0)
		OnCmdPRP(argc, argv, ptr);
	else if (strcasecmp(cmd, "CHG") == 0)
		OnCmdCHG(argc, argv, ptr);
	else if (strcasecmp(cmd, "ILN") == 0)
		OnCmdILN(argc, argv, ptr);
	else if (strcasecmp(cmd, "XFR") == 0)
		OnCmdXFR(argc, argv, ptr);
	else if (strcasecmp(cmd, "NLN") == 0)
		OnCmdNLN(argc, argv, ptr);
	else if (strcasecmp(cmd, "RNG") == 0)
		OnCmdRNG(argc, argv, ptr);
	else if (strcasecmp(cmd, "FLN") == 0)
		OnCmdFLN(argc, argv, ptr);
	else
		logger_warn("unkown cmd(%s), args(%s)", cmd, ptr);

	acl_argv_free(args);
}

// [VER] 1 MSNP21
// 协商MSN Messenger协议版本
void CMsnNSClient::OnCmdVER(int argc, const char** argv, const char* src)
{
	if (argc < 2)
	{
		logger_warn("unkown args: %s", src);
		client_->OnNSErr();
		return;
	}
	else if (strncasecmp(argv[1], "MSNP", 4) != 0)
	{
		logger_warn("invalid: %s, unkown args: %s", argv[1], src);
		client_->OnNSErr();
		return;
	}

	const char* ptr = argv[1];
	ptr += 4;
	int nver = atoi(ptr);
	if (nver <= 0)
	{
		logger_warn("invalid version: %s, args: %s", argv[1], src);
		client_->OnNSErr();
		return;
	}

	protocol_ver_ = nver;

	acl::string msg("CVR ");
	msg.format("CVR %d 0x0804 winnt 6.1.1 i386 MSNMSGR 8.5.1302 BC01 %s",
		client_->Sid(), account_.c_str());

	if (nver < 18)
		msg << "\r\n";
	else
	{
		acl::string ver;
		const char ver_txt[] = "Version: 1\r\nXfrCount: 1";
		ver.base64_encode(ver_txt, sizeof(ver_txt) - 1);
		msg.format_append(" %s\r\n", ver.c_str());
	}

	Send(msg.c_str(), msg.length());
}

// [CVR] 2 14.0.8117 14.0.8117 14.0.8117 http://msgruser.dlservice.microsoft.com/download/A/6/1/A616CCD4-B0CA-4A3D-B975-3EDB38081B38/fr/wlsetup-cvr.exe http://download.live.com/?sku=messenger
// 发出客户端的OS、语言、MSN Messenger版本等信息
void CMsnNSClient::OnCmdCVR(int argc, const char** argv, const char* src)
{
	if (argc < 2)
	{
		logger_warn("unkown args: %s", src);
		client_->OnNSErr();
	}

	acl::string msg;
	msg.format("USR %d SSO I %s\r\n", client_->Sid(), account_.c_str());
	Send(msg.c_str(), msg.length());
}

// [GCF] 0 5700
void CMsnNSClient::OnCmdGCF(int argc, const char** argv, const char* src)
{
	if (argc < 2)
	{
		logger_warn("GCF(%s) invalid", src);
		client_->OnNSErr();
		return;
	}
	
	gcf_length_ = atoi(argv[1]);
	if (gcf_length_ <= 0)
	{
		logger_warn("length(%d) invalid, args(%s)", gcf_length_, src);
		client_->OnNSErr();
		return;
	}

	status_ = NS_STATUS_GCF;
	gcf_read_ = 0;
	Read(gcf_length_);
}

// 声明、传递、鉴别用户身份
void CMsnNSClient::OnCmdUSR(int argc, const char** argv, const char* src)
{
	if (argc <= 3)
	{
		logger_error("argc(%d) invalid", argc);
		client_->OnNSErr();
		return;
	}

	// [USR] 3 SSO S MBI_KEY hH55hFAw1rt83hwAGNdtb26ApR62++FO4Rg7UJfAKkBTmXCCRpfu1egzBVYn8UHi
	if (strcasecmp(argv[1], "SSO") == 0)
	{
		if (argc < 5 || argv[2][0] != 'S' ||
			strcasecmp(argv[3], "MBI_KEY") != 0)
		{
			logger_warn("USR(%s) invalid", src);
		}
		else
		{
			mbi_key_ = argv[3];
			nonce_ = argv[4];
			client_->OnNSUsrSSO();
		}
	}

	// [USR] 4 OK forward_day@hotmail.com 1 0
	else if (strcasecmp(argv[1], "OK") == 0 && argc >= 3)
	{
		acl::string myaccount = argv[2];
		if (myaccount != account_)
		{
			logger_error("account(%s) is not mine", myaccount.c_str());
			client_->OnNSErr();
		}
		else
			printf(">>>>>>>>>login sso ok now<<<<<<<<<<<<<<<\r\n");
	}
	else
	{
		printf("unknown USR 6 %s\n", argv[1]);
	}
}

// [CHL] 0 8246214479184398201771905677
// 服务器发出验证要求
void CMsnNSClient::OnCmdCHL(int argc, const char** argv, const char* src)
{
	if (argc < 2)
	{
		logger_error("argc(%s) invalid", argc);
		client_->OnNSErr();
		return;
	}

	const char* ptr = argv[1];
	CMsnClientInfo info;
	char qry_key[33];

	CMsnQRY::QRYKey(info.product_id_.c_str(),
		info.product_key_.c_str(), ptr, qry_key);

	acl::string query;
	query.format("QRY %d %s 32\r\n%s\r\n",
		client_->Sid(), info.product_id_.c_str(), qry_key);
	Send(query.c_str(), query.length());
}

// [SBS] 0 null
void CMsnNSClient::OnCmdSBS(int argc, const char** argv, const char* src)
{

}

// [MSG] Hotmail Hotmail 293
// 传递服务器(系统) 的消息到客户端
void CMsnNSClient::OnCmdMSG(int argc, const char** argv, const char* src)
{
	remote_user_.clear(); // 先清除 MSN 消息发送者

	if (argc < 3)
	{
		logger_error("argc(%s) invalid", argc);
		client_->OnNSErr();
		return;
	}

	msg_length_ = atoi(argv[2]);
	if (msg_length_ <= 0)
	{
		logger_error("msg_length(%s) invalid", argv[2]);
		client_->OnNSErr();
		return;
	}

	remote_user_ = argv[1]; // 记录当前 MSG 消息的发送者

	// 设置当前 IO 状态，以便于在 OnRead 中能切换到 OnMsgData 过程
	status_ = NS_STATUS_MSG;

	// 收到 MSG 类型的命令字后，需要重置 MIME 解析器，以开始
	// 下一个 MIME 数据解析过程
	mime_.reset();

	msg_read_ = 0;  // 记录已读数据的长度

	// 读取 MSG 的数据体
	Read(msg_length_);
}

// [UBX] forward_day@hotmail.com 1 614
void CMsnNSClient::OnCmdUBX(int argc, const char** argv, const char* src)
{
	if (argc < 3)
	{
		logger_error("argc(%s) invalid, args: %s", argc, src);
		client_->OnNSErr();
		return;
	}
	printf(">>argc: %d, %s, %s, %s\n", argc, argv[0], argv[1], src);

	msg_length_ = atoi(argv[2]);
	if (msg_length_ <= 0)
	{
		// 切换至命令状态
		status_ = NS_STATUS_CMD;
		Gets();
		return;
	}

	msg_read_ = 0;

	// 设置当前 IO 状态，以便于在 OnRead 中能切换到 OnMsgData 过程
	status_ = NS_STATUS_UBX;

	// 读取 UBX 的数据体
	Read(msg_length_);
}

void CMsnNSClient::OnUbxData(char* data, int len)
{
	acl_assert(len == msg_length_);

	printf(">>(%s)\n", data);

	// 切换至命令状态
	status_ = NS_STATUS_CMD;
	Gets();
}

// [QRY] 5
// 客户端回答服务器的验证要求
void CMsnNSClient::OnCmdQRY(int argc, const char** argv, const char* src)
{
	//contactManager_.PrintUsers();
}

void CMsnNSClient::OnCmdADL(int argc, const char** argv, const char* src)
{
	adl_fqy_--;
	if (adl_fqy_ == 0)  // 说明登入成功
		FinishLogin();
}

// 设置对尚未列入明确允许/禁止的联系人列表的保密策略
void CMsnNSClient::OnCmdBLP(int argc, const char** argv, const char* src)
{

}

// 发出设置个人电话号码的请求
void CMsnNSClient::OnCmdPRP(int argc, const char** argv, const char* src)
{

}

// 发出改变状态的请求
void CMsnNSClient::OnCmdCHG(int argc, const char** argv, const char* src)
{

}

// 登入后，此命令表明了在线用户
// [ILN] 0 NLN/IDL/BSY zhengshuxin@hotmail.com 1 display_name
void CMsnNSClient::OnCmdILN(int argc, const char** argv, const char* src)
{
	if (argc < 5)
	{
		logger_warn("ILN(%s) invalid", src);
		return;
	}

	const char* status = argv[1];
	const char* passport = argv[2];
	const char* display_name = argv[4];
	Member* member = contactManager_.SetUserStatus(passport,
		status, display_name);

	//printf("OnCmdILN >>>Display: %s, passport: %s, status: %s\r\n",
	//	member->DisplayName, member->PassportName,
	//	StatusToString(member->online_));
}

// [XFR] 23 SB 64.4.61.70:1863 CKI 1314535075.121206146.2142811 U messenger.msn.com 1
void CMsnNSClient::OnCmdXFR(int argc, const char** argv, const char* src)
{
	printf(">>src(%s)\n", src);
	if (argc < 5)
	{
		logger_error("argc(%d) invalid, src: %s", argc, src);
		return;
	}
	int id = atoi(argv[0]);
	if (id < 0)
	{
		logger_error("id(%d) invalid, src: %s", id, src);
		return;
	}
	else if (strcasecmp(argv[1], "SB") != 0 || strcasecmp(argv[3], "CKI") != 0)
	{
		logger_error("src(%s) invalid", src);
		return;
	}

	const char* addr = argv[2];
	const char* auth_key = argv[4];

	CMsnSwitchBoard* sb = GetSwitchBoard(id);
	sb->SetAuthKey(auth_key);
	sb->Open(addr, 120);
}

// 通知客户端联系人上线或改变状态
void CMsnNSClient::OnCmdNLN(int argc, const char** argv, const char* src)
{
	printf(">>src(%s)\n", src);
}

// 被动模式，收到其他人的聊天邀请
// RNG: RINGING
// [RNG] session_id ip:port CKI auth_key from_user from_user U messenger 1
// [RNG] 853045962 64.4.61.152:1863 CKI 138218181.19286130 zhengshuxin@hotmail.com zhengshuxin@hotmail.com U messenger.msn.com 1
void CMsnNSClient::OnCmdRNG(int argc, const char** argv, const char* src)
{
	if (argc < 5)
	{
		logger_warn("RNG(%s) invalid", src);
		return;
	}
	const char* sid = argv[0];
	const char* addr = argv[1];
	const char* auth_key = argv[3];
	const char* from_user = argv[4];

	CMsnSwitchBoard* sb = GetSwitchBoard(from_user);

	sb->SetSid(sid);
	sb->SetAuthKey(auth_key);

	// 设置为被邀请模式
	sb->SetInvited(true);
	// 打开与 switchborad 服务器的连接
	sb->Open(addr, 120);
}

// [FLN] zhengshuxin@hotmail.com 1 0
void CMsnNSClient::OnCmdFLN(int argc, const char** argv, const char* src)
{
	if (argc < 1)
	{
		logger_error("argc(%d) invalid, src: %s", argc, src);
		return;
	}
}

void CMsnNSClient::OnGcfData(char* data, int len)
{
	int  n;

	gcf_read_ += len;
	n = gcf_length_ - gcf_read_;
	gcf_body_.update(data);
	if (n == 0)
	{
		// 切换至命令状态
		status_ = NS_STATUS_CMD;
		Gets();
	}
	else if (n > 0)
		Read(n);
	else
		printf(">>>error: n: %d\r\n", n);
}

/*
MSG Hotmail Hotmail 289
MIME-Version: 1.0
Content-Type: text/x-msmsgsinitialmdatanotification; charset=UTF-8

Mail-Data: <MD><E><I>0</I><IU>0</IU><O>0</O><OU>0</OU></E><Q><QTM>409600</QTM><QNM>204800</QNM></Q></MD>
Inbox-URL: /cgi-bin/HoTMaiL
Folders-URL: /cgi-bin/folders
Post-URL: http://www.hotmail.com
*/
void CMsnNSClient::OnMsgData(char* data, int len)
{
	acl_assert(len == msg_length_);

	mime_.reset();
	ACL_VSTRING* vbuf = acl_vstring_alloc(256);
	ssize_t   n = len;
	const char* pdata = data, *plast;
	while(len > 0)
	{
		ACL_VSTRING_RESET(vbuf);
		plast = pdata;
		if (acl_buffer_gets(vbuf, &pdata, len) == NULL
			|| strcmp(acl_vstring_str(vbuf), "\r\n") == 0
			|| strcmp(acl_vstring_str(vbuf), "\n") == 0)
		{
			len -= (ssize_t) (pdata - plast);
			mime_.update("\r\n", 2);
			break;
		}

		if (strstr(acl_vstring_str(vbuf), "Nickname"))
			printf(">>>%s\r\n", acl_vstring_str(vbuf));
		n = (ssize_t) (pdata - plast);
		len -= n;
		mime_.update(plast, n);
	}
	mime_.update_end();
	acl_vstring_free(vbuf);

	const char* content_type = mime_.header_value("Content-Type");
	if (content_type == NULL)
	{
		logger_error("no Content-Type found");
		client_->OnNSErr();
		return;
	}

	printf(">>>content_type: %s\r\n", content_type);

	ACL_ARGV* tokens = acl_argv_split(content_type, ";");
	ACL_ITER iter;
	acl_foreach(iter, tokens)
	{
		const char* ptr = (const char*) iter.data;
		int  argc = tokens->argc;
		const char** argv = (const char**) tokens->argv;

		if (strcasecmp(ptr, "text/x-msmsgsprofile") == 0)
		{
			OnMsgProfile(argc, argv);
			break;
		}

		// initial OIM notification
		else if (strcasecmp(ptr, "text/x-msmsgsinitialmdatanotification") == 0)
		{
			OnMsgInitialMdata(argc, argv, pdata, len);
			break;
		}

		// OIM notification when user online
		else if (strcasecmp(ptr, "text/x-msmsgsoimnotification") == 0)
		{
			OnMsgInitialEmail(argc, argv);
			break;
		}

		else if (strcasecmp(ptr, "text/x-msmsgsinitialemailnotification") == 0)
		{
			OnMsgInitialEmail(argc, argv);
			break;
		}

		else if (strcasecmp(ptr, "text/x-msmsgsemailnotification") == 0)
		{
			OnMsgEmail(argc, argv);
			break;
		}

		// delete an offline Message notification
		// Now this is the unread OIM info, not the new mail.
		else if (strcasecmp(ptr, "text/x-msmsgsactivemailnotification") == 0)
		{
			OnMsgDelOim(argc, argv);
			break;
		}

		else if (strcasecmp(ptr, "application/x-msmsgssystemmessage") == 0)
		{
			OnMsnMsgSystem(argc, argv);
			break;
		}

		// generic message handlers
		else if (strcasecmp(ptr, "text/plain") == 0)
		{
			OnMsnMsgPlain(argc, argv);
			break;
		}

		else if (strcasecmp(ptr, "text/x-msmsgscontrol") == 0)
		{
			OnMsnMsgControl(argc, argv);
			break;
		}

		else if (strcasecmp(ptr, "text/x-msnmsgr-datacast"))
		{
			OnMsnMsgDataCast(argc, argv);
			break;
		}
		else
			printf(">>>>>unknown content-type: %s\n", ptr);
	}

	// 切换至命令状态
	status_ = NS_STATUS_CMD;
	Gets();
}

void CMsnNSClient::OnMsgProfile(int argc, const char** argv)
{
	const char* ptr = mime_.header_value("sid");
	if (ptr)
		passport_.sid_ = ptr;

	ptr = mime_.header_value("MSPAuth");
	if (ptr)
		passport_.mspauth_ = ptr;

	ptr = mime_.header_value("ClientIP");
	if (ptr)
		passport_.client_ip_ = ptr;

	ptr = mime_.header_value("ClientPort");
	if (ptr)
		passport_.client_port = ntohs(atoi(ptr));

	ptr = mime_.header_value("LoginTime");
	if (ptr)
		passport_.login_time_ = atol(ptr);

	ptr = mime_.header_value("EmailEnabled");
	if (ptr && atol(ptr))
		passport_.email_enable_ = true;
	else
		passport_.email_enable_ = false;

	ptr = mime_.header_value("NickName");
	if (ptr)
	{
		if (conv_.convert("UTF-8", "GBK", ptr, strlen(ptr),
			&passport_.nickname_) == false)
		{
			printf("convert nick error\r\n");
		}
		else
			printf("convert nick ok: %s\r\n", passport_.nickname_.c_str());
		printf(">>nickname: %s, len: %d, ptr: %s, len: %d\r\n",
			passport_.nickname_.c_str(), (int) strlen(ptr), ptr, strlen(ptr));
		//passport_.nickname_ = "zsxxsz";
		//passport_.nickname_ = ptr;
	}

	ptr = mime_.header_value("RouteInfo");
	if (ptr)
		passport_.route_info_ = ptr;

	//////////////////////////////////////////////////////////////////////////

	acl_assert(ticket_);

	// 开始获取联系人列表
	client_->GetService().start_get_contact(this, *ticket_);
}

// 获得联系人完毕后的回调函数
void CMsnNSClient::OnGetContactFinish(bool ok)
{
	if (!ok)
	{
		logger_error("GetContact error");
		return;
	}
	SendPrivacy();  // 设置个人属性
	DumpContact();  // 向NS服务器发送联系人信息
}

void CMsnNSClient::SendPrivacy()
{
	acl::string msg("BLP ");  // 设置对尚未列入明确允许/禁止的联系人列表的保密策略
	msg << client_->Sid() << " AL\r\n";
	Send(msg.c_str(), msg.length());
}

void CMsnNSClient::DumpContact()
{
	const std::list<CMembership*>& memberShips =
		contactManager_.GetMemberships();

#if 1
	acl::string buf;
	CMsnADLPayload::ToString(client_, buf, memberShips);
	//acl::string hdr("ADL ");  // ADL: ADD LIST
	//hdr << client_->Sid() << " " << (int) buf.length() << "\r\n";
	//hdr << "PRP 10 MFN 郑树新--视频加速器：http://v.hexun.com/DownloadNew.htm\r\n";

	//Send(hdr.c_str(), hdr.length());
	Send(buf.c_str(), buf.length());

#else

	const char* hdr1 = "ADL 9 5185\r\n";
	acl::string data1;
	data1 << "<ml l='1'><d n='live.cn'><c n='jipiao4070' l='4' t='1'/>"
		<< "<c n='tangjianling' l='3' t='1'/></d>"
		<< "<d n='msn.cn'><c n='hiuaw03n5rgxqg8' l='5' t='1'/></d>"
		<< "<d n='msnzone.cn'><c n='group316871' l='5' t='1'/>"
		<< "<c n='group179246' l='4' t='1'/></d>"
		<< "<d n='gmail.com'><c n='weixiaolei' l='5' t='1'/></d>"
		<< "<d n='hotmail.com'><c n='mgroup19020' l='4' t='1'/>"
		<< "<c n='lovejunyan167' l='4' t='1'/><c n='fangyin_kiss329' l='4' t='1'/>"
		<< "<c n='liuxing_ie849' l='4' t='1'/><c n='zhaoliang108' l='5' t='1'/>"
		<< "<c n='tns_bial_434731' l='4' t='1'/><c n='yimin_i427' l='4' t='1'/>"
		<< "<c n='qili_7151' l='4' t='1'/><c n='wangjing_5580' l='4' t='1'/>"
		<< "<c n='feixue_chen603' l='4' t='1'/><c n='littlepighaha' l='5' t='1'/>"
		<< "<c n='jiling_oe637' l='4' t='1'/><c n='yuxuan_love748' l='4' t='1'/>"
		<< "<c n='lixinjie_910' l='4' t='1'/><c n='Zenobia_pot210' l='4' t='1'/>"
		<< "<c n='yinyin_6520' l='4' t='1'/><c n='jiqin_happy0622' l='4' t='1'/>"
		<< "<c n='Margaret_ni31' l='4' t='1'/><c n='baixin_2321' l='4' t='1'/>"
		<< "<c n='rern_wang3439' l='4' t='1'/><c n='ajhghegcnjg1132' l='4' t='1'/>"
		<< "<c n='ince040' l='4' t='1'/><c n='wenliang_du' l='3' t='1'/>"
		<< "<c n='xinlly' l='3' t='1'/><c n='yecao06' l='3' t='1'/>"
		<< "<c n='lijun_foundir' l='3' t='1'/><c n='czq' l='3' t='1'/>"
		<< "<c n='liji2046' l='3' t='1'/><c n='weijia789' l='3' t='1'/>"
		<< "<c n='zhouling922' l='3' t='1'/><c n='xocecd' l='3' t='1'/>"
		<< "<c n='evawu596' l='3' t='1'/><c n='qyzhou1982' l='3' t='1'/>"
		<< "<c n='xu_xueyi2005' l='3' t='1'/><c n='tang_yuliang' l='3' t='1'/>"
		<< "<c n='xiaofeng_feng' l='3' t='1'/><c n='oui777333' l='3' t='1'/>"
		<< "<c n='whoissimon' l='3' t='1'/><c n='egt_tangjianling' l='3' t='1'/>"
		<< "<c n='zhouj0706' l='3' t='1'/><c n='chx4207' l='3' t='1'/>"
		<< "<c n='cxhongyh' l='3' t='1'/><c n='jxiaohua1219' l='3' t='1'/>"
		<< "<c n='lucy_zhangyun' l='3' t='1'/><c n='syx007766' l='3' t='1'/>"
		<< "<c n='avalon_200000' l='3' t='1'/><c n='yu_hua_zhang' l='3' t='1'/>"
		<< "<c n='liuwei_love' l='3' t='1'/><c n='speedian' l='3' t='1'/>"
		<< "<c n='zhangxh0618' l='3' t='1'/><c n='chorwa' l='3' t='1'/>"
		<< "<c n='cavatina2007' l='3' t='1'/><c n='shifushuai' l='3' t='1'/>"
		<< "<c n='llqwpf' l='3' t='1'/><c n='kirk1368' l='3' t='1'/>"
		<< "<c n='forward_day' l='3' t='1'/><c n='kaixinqiqi' l='3' t='1'/>"
		<< "<c n='henrydai1973' l='3' t='1'/><c n='jordan23pippen33' l='3' t='1'/>"
		<< "<c n='Dextrad19830105' l='3' t='1'/><c n='bigeyesyy' l='3' t='1'/>"
		<< "<c n='zeus_prince' l='3' t='1'/><c n='li_chong1977' l='3' t='1'/>"
		<< "<c n='limboftime' l='3' t='1'/><c n='shenhui13' l='3' t='1'/>"
		<< "<c n='jecyzyq' l='3' t='1'/><c n='sun_ge' l='3' t='1'/>"
		<< "<c n='mhmjia' l='3' t='1'/><c n='qiaoyongdv' l='3' t='1'/>"
		<< "<c n='willwill1981' l='3' t='1'/><c n='cy8864' l='3' t='1'/>"
		<< "<c n='hotin2005' l='3' t='1'/><c n='enze_cui' l='3' t='1'/>"
		<< "<c n='davidchai9121' l='3' t='1'/><c n='zzylq' l='3' t='1'/>"
		<< "<c n='stephen800413' l='3' t='1'/><c n='rao_yi' l='3' t='1'/>"
		<< "<c n='error_msg' l='3' t='1'/><c n='hjyhit' l='3' t='1'/>"
		<< "<c n='wjw_xyz' l='3' t='1'/><c n='dhldu' l='3' t='1'/>"
		<< "<c n='joyheros' l='3' t='1'/><c n='huataow' l='3' t='1'/>"
		<< "<c n='liangzhuo79' l='3' t='1'/><c n='you_full' l='3' t='1'/>"
		<< "<c n='maosengao' l='3' t='1'/><c n='huifeng.wang' l='3' t='1'/>"
		<< "<c n='juyingxi' l='3' t='1'/><c n='fenghao3435' l='3' t='1'/>"
		<< "<c n='gw_2005' l='3' t='1'/><c n='dragonqlw' l='3' t='1'/>"
		<< "<c n='nhccjsj' l='3' t='1'/><c n='wuyu_2001' l='3' t='1'/>"
		<< "<c n='dragonwang496' l='3' t='1'/><c n='navychen2003' l='3' t='1'/>"
		<< "<c n='zhaopengzheng' l='3' t='1'/><c n='sherrysliding' l='3' t='1'/>"
		<< "<c n='brush_7color' l='3' t='1'/><c n='cuijg2008' l='3' t='1'/>"
		<< "<c n='zmm76' l='3' t='1'/><c n='martinzhaobj' l='3' t='1'/>"
		<< "<c n='sjzhang75' l='3' t='1'/><c n='zxydawn' l='3' t='1'/></d>"
		<< "<d n='msn.com'><c n='zkxiao' l='5' t='1'/><c n='kakluote' l='4' t='1'/>"
		<< "<c n='sjchtb' l='3' t='1'/><c n='yd_hx' l='3' t='1'/>"
		<< "<c n='pineweb' l='3' t='1'/><c n='wenyi775' l='3' t='1'/>"
		<< "<c n='newheart1' l='3' t='1'/><c n='raymond_x' l='3' t='1'/></d>"
		<< "<d n='9zi.com'><c n='bot135' l='4' t='1'/><c n='bot112' l='4' t='1'/></d>"
		<< "<d n='LIVE.COM'><c n='xuheng' l='3' t='1'/></d>"
		<< "<d n='sunlike.com.cn'><c n='sl.huang' l='3' t='1'/></d>"
		<< "<d n='yahoo.com'><c n='shuxin.zheng' l='3' t='32'/>"
		<< "<c n='zeng_hongwei' l='3' t='32'/><c n='shuangyan1120' l='3' t='32'/>"
		<< "<c n='dire2008' l='3' t='1'/></d><d n='163.com'>"
		<< "<c n='xinghan79' l='3' t='1'/></d><d n='foundir.com'>"
		<< "<c n='huifeng.wang' l='3' t='1'/></d><d n='21cn.com'>"
		<< "<c n='coogle' l='3' t='1'/></d><d n='sohu.com'>"
		<< "<c n='canphie' l='3' t='1'/><c n='beyond_cjt' l='3' t='1'/>"
		<< "<c n='computer11111' l='3' t='1'/></d><d n='yahoo.cn'>"
		<< "<c n='qinyan' l='3' t='1'/></d><d n='189.cn'><c n='13391932087' l='3' t='1'/></d><d n='biible.net'><c n='bbn' l='3' t='1'/></d><d n='mx.cei.gov.cn'><c n='cyz' l='3' t='1'/></d><d n='263.sina.com'><c n='brave_man' l='3' t='1'/></d><d n='tom.com'><c n='loversmc' l='3' t='1'/></d><d n='vip.sina.com'><c n='lwj213' l='3' t='1'/></d><d n='cmhk.com'><c n='szf' l='3' t='1'/><c n='fangw' l='3' t='1'/></d><d n='x263.net'><c n='seoul.cai' l='3' t='1'/><c n='youjia' l='3' t='1'/></d><d n='126.com'><c n='junyao2008' l='3' t='1'/><c n='lina_vc' l='3' t='1'/><c n='caroline0925' l='3' t='1'/></d><d n='sina.com'><c n='littlesunsun' l='3' t='1'/><c n='xushunying' l='3' t='1'/><c n='jetying29' l='3' t='1'/></d><d n='yahoo.com.cn'><c n='junxing_882004' l='3' t='1'/></d><d n='263.net'><c n='thliang' l='3' t='1'/></d></ml>";
	Send(hdr1, strlen(hdr1));
	Send(data1, strlen(data1));
	const char* hdr2 = "ADL 10 3561\r\n";
	acl::string data2;
	data2 << "<ml l='1'><d n='hotmail.com'><c n='shark_yang' l='3' t='1'/>"
		<< "<c n='suran007' l='3' t='1'/><c n='guance_ms' l='3' t='1'/><c n='chenfeng-008' l='3' t='1'/><c n='kiddy_1999' l='3' t='1'/><c n='xinghua' l='3' t='1'/><c n='kangjifangnew' l='3' t='1'/><c n='waullll' l='3' t='1'/><c n='shuzheng_yang_foundir' l='3' t='1'/><c n='chengyuzhuang' l='3' t='1'/><c n='yangfl111' l='3' t='1'/><c n='asdasd99_asd' l='3' t='1'/><c n='lim71' l='3' t='1'/>"
		<< "<c n='yaayh_our' l='3' t='1'/><c n='turbonet.com' l='3' t='1'/><c n='liubincn' l='3' t='1'/><c n='victorboy45' l='3' t='1'/><c n='zds_hn' l='3' t='1'/><c n='lj_0419' l='3' t='1'/><c n='xiaxue_snow' l='3' t='1'/><c n='nxwzhl' l='3' t='1'/><c n='disaxiyu' l='3' t='1'/><c n='sunluming' l='3' t='1'/><c n='haiguangliu' l='3' t='1'/><c n='MaShiMin' l='3' t='1'/><c n='wincardwang' l='3' t='1'/>"
		<< "<c n='bjweiqiong' l='3' t='1'/><c n='ganquan76' l='3' t='1'/><c n='songyanguw' l='3' t='1'/><c n='history_kitty' l='3' t='1'/><c n='prosoftwarenet' l='3' t='1'/><c n='luowantao' l='3' t='1'/><c n='tiankonganjing_1024' l='3' t='1'/><c n='springapril73' l='3' t='1'/><c n='senondeng' l='3' t='1'/><c n='snapbug' l='3' t='1'/><c n='kezhi_fu' l='3' t='1'/><c n='yl8546' l='3' t='1'/><c n='yangrui5325' l='3' t='1'/><c n='jiechen_551' l='3' t='1'/>"
		<< "<c n='ysz0828' l='3' t='1'/><c n='pentiumpoon' l='3' t='1'/><c n='chyh00' l='3' t='1'/><c n='yangfang359' l='3' t='1'/><c n='zhengqj88' l='3' t='1'/><c n='namelysweet' l='3' t='1'/><c n='renyi0746' l='3' t='1'/><c n='ell_tearn' l='3' t='1'/><c n='blue.scorpions' l='3' t='1'/><c n='shawnshi03' l='3' t='1'/><c n='xlfirst' l='3' t='1'/><c n='moresong688' l='3' t='1'/><c n='jianxing_chen' l='3' t='1'/>"
		<< "<c n='hexiaoyihexiaoyi' l='3' t='1'/><c n='qfls001' l='3' t='1'/><c n='xu_zhao_foundir' l='3' t='1'/><c n='kankaijun' l='3' t='1'/><c n='woody0313' l='3' t='1'/><c n='fengweichen' l='3' t='1'/><c n='guichuan' l='3' t='1'/><c n='bluecms' l='3' t='1'/><c n='jingxin.zhang' l='3' t='1'/><c n='zhanggage' l='3' t='1'/><c n='hugusu' l='3' t='1'/><c n='sionliu' l='3' t='1'/><c n='wuyujava' l='3' t='1'/><c n='joyjava' l='3' t='1'/><c n='synbada' l='3' t='1'/>"
		<< "<c n='cbuiler_guo' l='3' t='1'/><c n='fjliuwenliang' l='3' t='1'/><c n='hotmail_qingguo' l='3' t='1'/><c n='lulin315' l='3' t='1'/><c n='jingearth' l='3' t='1'/><c n='fish.102' l='3' t='1'/><c n='kingyee1999' l='3' t='1'/><c n='wbc_40' l='3' t='1'/><c n='zhangyang721123' l='3' t='1'/></d><d n='msn.com'><c n='mars_maya' l='3' t='1'/><c n='yeahfox' l='3' t='1'/><c n='aqjh' l='3' t='1'/><c n='eversprint' l='3' t='1'/><c n='mgd_75' l='3' t='1'/>"
		<< "<c n='land_beijing' l='3' t='1'/><c n='leiying3385' l='3' t='1'/><c n='yukaizhao' l='3' t='1'/></d><d n='staff.monternet.com'><c n='qianhc' l='3' t='1'/></d><d n='163.com'><c n='chnl' l='3' t='1'/><c n='hang.hao' l='3' t='1'/></d><d n='live.cn'><c n='zhangjiashu' l='3' t='1'/><c n='carol.du' l='3' t='1'/><c n='chyh00' l='3' t='1'/></d><d n='ancs.com.cn'><c n='mujw' l='3' t='1'/></d><d n='gmail.com'><c n='icarusster' l='3' t='1'/></d>"
		<< "<d n='net263.com'><c n='ruihua.zhang' l='3' t='1'/></d><d n='china.com'><c n='aoliewei' l='3' t='1'/></d><d n='microtalent.com.cn'><c n='jasonwang' l='3' t='1'/></d><d n='japer.net'><c n='admin' l='3' t='1'/></d><d n='126.com'><c n='zhaofuyun202' l='3' t='1'/><c n='yangzi2008' l='3' t='1'/></d><d n='live.com'><c n='aixing' l='3' t='1'/></d>"
		<< "<d n='263.net'><c n='csaga' l='3' t='1'/></d><d n='foundir.com'><c n='xingyu.liu' l='3' t='1'/></d><d n='sms107.com'><c n='smsbot525' l='2' t='1'/></d><d n='sohu-inc.com'><c n='jansomwang' l='2' t='1'/></d></ml>";
	Send(hdr2, strlen(hdr2));
	Send(data2, strlen(data2));
	// 增加 ADL 引用计数
	adl_fqy_++;
#endif

	adl_fqy_++;

	// 设置显示名称
	SetPublicAlias();
}

// 设置显示昵称
void CMsnNSClient::SetPublicAlias()
{
	if (passport_.nickname_.empty())
		return;
	acl::string msg("PRP ");
	//msg << client_->Sid() << " MFN " << passport_.nickname_.c_str() << "\r\n";

	// 字符集转换成 utf-8
	acl::string buf;
	const char* name = "郑树新";
	acl::charset_conv conv;
	bool ret = conv.convert("GB18030", "UTF-8", name,
		strlen(name), &buf);
	msg << client_->Sid() << " MFN " << buf.c_str() << "\r\n";
	Send(msg.c_str(), msg.length());
}

void CMsnNSClient::OnMsgInitialMdata(int argc, const char** argv, const char* ptr, size_t n)
{
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
	if (remote_user_.ncompare("Hotmail", sizeof("Hotmail") - 1, false) != 0)
	{
		logger_warn("unknown remote user: %s", remote_user_.c_str());
		return;
	}
	else if (ptr == NULL || n <= 0)
	{
		logger_warn("no mime data");
		return;
	}

	acl::mime mime;
	mime.update(ptr, n);
	mime.update("\r\n", 2);
	mime.update_end();
	// 分析离线消息
	oim_ = CMsnOim::Create(mime);

	//acl::string xfr("XFR ");
	//xfr << client_->Sid() << " SB\r\n";
	//Send(xfr.c_str(), xfr.length());
}

void CMsnNSClient::OnMsgInitialEmail(int argc, const char** argv)
{

}

void CMsnNSClient::OnMsgEmail(int argc, const char** argv)
{

}

void CMsnNSClient::OnMsgDelOim(int argc, const char** argv)
{

}

void CMsnNSClient::OnMsnMsgSystem(int argc, const char** argv)
{

}

void CMsnNSClient::OnMsnMsgPlain(int argc, const char** argv)
{

}

void CMsnNSClient::OnMsnMsgControl(int argc, const char** argv)
{

}

void CMsnNSClient::OnMsnMsgDataCast(int argc, const char** argv)
{

}

void CMsnNSClient::OnSend()
{

}

void CMsnNSClient::OnTimeout()
{

}

void CMsnNSClient::OnClose()
{
	client_->OnNSClose();
	delete this;
	printf(">>>NS CLIENT DELETE NOW<<<\r\n");
}

void CMsnNSClient::Login(const CMsnTicket* ticket)
{
	ticket_ = ticket;

	const TICKET* tt = ticket->GetTicket("messengerclear.live.com");
	acl::string respond;
	CMsnUsrKey usrKey;
	usrKey.CreateKey(tt->secret, nonce_.c_str(), respond);

	acl::string msg("USR 7 SSO S t=");
	msg << tt->ticket << "&p=";
	if (tt->p)
		msg << tt->p;
	msg << " " << respond.c_str();

	if (protocol_ver_ < 16)
		msg << " " << MSN_APPLICATION_ID << "\r\n";
	else
		msg << " {" << MSN_APPLICATION_ID << "}\r\n";

	Send(msg.c_str(), msg.length());
}

void CMsnNSClient::FinishLogin()
{
	logged_in_ = true;

	MsnClientCaps caps = (MsnClientCaps) MSN_CLIENT_ID;

	acl::string msg;
	// 更新在线状态
	msg.format("CHG %d NLN %u\r\n", client_->Sid(), (unsigned int) caps);
	Send(msg.c_str(), msg.length());
}

CMsnSwitchBoard* CMsnNSClient::GetSwitchBoard(const char* peer_user)
{
	CMsnSwitchBoard* sb;
	acl::string key(peer_user);
	key.lower();

	std::map<acl::string, CMsnSwitchBoard*>::iterator it = sb_list_.find(key);
	if (it == sb_list_.end())
	{
		// 创建 CMsnSwitchBoard 类对象
		sb = NEW CMsnSwitchBoard(this, key.c_str());
		// 添加进 switchboard 的集合中
		sb_list_[key] = sb;
	}
	else
		sb = it->second;
	return (sb);
}

CMsnSwitchBoard* CMsnNSClient::GetSwitchBoard(int xfr_id)
{
	std::map<acl::string, CMsnSwitchBoard*>::iterator it = sb_list_.begin();
	for (; it != sb_list_.end(); it++)
	{
		if (it->second->GetXfrId() == xfr_id)
			return (it->second);
	}

	return (NULL);
}

void CMsnNSClient::DeleteSwitchBorard(CMsnSwitchBoard* sb)
{
	const acl::string& peer = sb->GetPeerUser();
	std::map<acl::string, CMsnSwitchBoard*>::iterator it = sb_list_.find(peer);
	if (it == sb_list_.end())
	{
		logger_warn("unknown peer user: %s", peer.c_str());
		return;
	}
	sb_list_.erase(it);
}

void CMsnNSClient::SpeakTo(const char* toUser, const char* data, size_t dlen)
{
	CMsnSwitchBoard* sb = GetSwitchBoard(toUser);

	sb->SetInvited(false); // 设置为主动模式
	if (sb->IsConnected())
	{
		sb->SendMsg(data);
		return;
	}

	sb->AppendMsg(data);
        int   xfr_id = client_->Sid();
	sb->SetXfrId(xfr_id);

	acl::string buf;
	buf.format("XFR %d SB\r\n", xfr_id);
	printf(">>buf(%s)\r\n", buf.c_str());
	Send(buf.c_str(), buf.length());
}