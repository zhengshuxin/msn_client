#include "stdafx.h"
#include "MsnGlobal.h"
#include "MsnNSClient.h"
#include ".\msnswitchboard.h"

//////////////////////////////////////////////////////////////////////////

CMsnSwitchBoard::CMsnSwitchBoard(CMsnNSClient* ns_client,
	const char* peer_user)
: CMsnChat("CMsnSwitchBoard")
, ns_client_(ns_client)
, peer_user_(peer_user)
, is_invited_(false)
, status_(SB_STATUS_CMD)
, current_users_(0)
, ready_(false)
, id_(1)
, xfr_id_(-1)
, is_connected_(false)
, is_quiting_(false)
{

}

CMsnSwitchBoard::~CMsnSwitchBoard(void)
{
	std::list<CMsnMsg*>::iterator it = chat_msgs_.begin();
	for (; it != chat_msgs_.end(); it++)
		delete *it;
}

void CMsnSwitchBoard::SetAuthKey(const char* auth_key)
{
	auth_key_ = auth_key;
}

void CMsnSwitchBoard::SetSid(const char* sid)
{
	sid_ = sid;
}

void CMsnSwitchBoard::Start()
{
	acl::string username;

	if (ns_client_->GetVersion() >= 16)
		username.format("%s;{%s}", ns_client_->GetAccount().c_str(),
			MSN_APPLICATION_ID);
	else
		username = ns_client_->GetAccount().c_str();

	acl::string msg;

	if (is_invited_)
		msg.format("ANS %d %s %s %s\r\n", id_++,
			username.c_str(), auth_key_.c_str(), sid_.c_str());
	else
		msg.format("USR %d %s %s\r\n", id_++,
			username.c_str(), auth_key_.c_str());

	// 发送消息
	Send(msg.c_str(), msg.length());

	// 异步接收消息
	Gets(true);
}

void CMsnSwitchBoard::SendMsg(const char* data)
{
	acl_assert(is_connected_);

	acl::string msg_body, buf;

	conv_.convert("GBK", "UTF-8", data, strlen(data), &buf);
	const char* ptr = buf.c_str();

	msg_body.format("MIME-Version: 1.0\r\n"
		"Content-Type: text/plain; charset=UTF-8\r\n"
		"User-Agent: msn_client/9.11\r\n"
		"X-MMS-IM-Format: FN=Segoe%%20UI; EF=; CO=0; PF=0; RL=0\r\n\r\n%s",
		ptr);
	acl::string msg_hdr;
	msg_hdr.format("MSG %d A %d\r\n", id_++, (int) msg_body.length());
	Send(msg_hdr.c_str(), msg_hdr.length());
	Send(msg_body.c_str(), msg_body.length());
}

void CMsnSwitchBoard::SendMsg(CMsnMsg* msg)
{
	acl_assert(is_connected_);
	SendMsg(msg->GetData().c_str());
}

void CMsnSwitchBoard::AppendMsg(const char* data)
{
	acl_assert(!is_connected_);
	CMsnMsg* msg = NEW CMsnMsg(data, strlen(data)); 
	chat_msgs_.push_back(msg);
}

void CMsnSwitchBoard::SetInvited(bool yesno /* = true */)
{
	is_invited_ = yesno;
}

bool CMsnSwitchBoard::IsInvited(void) const
{
	return (is_invited_);
}

void CMsnSwitchBoard::Quit()
{
	is_quiting_ = true;
	Close();
}

void CMsnSwitchBoard::AddUser(const acl::string& user_in,
	const acl::string& display_name)
{
	acl::string passport = user_in.c_str(), out;
	passport.lower();
	conv_.convert("UTF-8", "GBK", display_name.c_str(),
		display_name.length(), &out);
	std::map<acl::string, acl::string>::iterator it =
		chat_users_.find(passport.c_str());
	if (it != chat_users_.end())
		it->second = out.c_str();
	else
		chat_users_[passport.c_str()] = out;

	current_users_++;
}

void CMsnSwitchBoard::OnOpen()
{
	//Debug(true); // only for test
	is_connected_ = true;
	Start();
}

void CMsnSwitchBoard::OnRead(char* data, int len)
{
	if (status_ == SB_STATUS_CMD)
		OnCmd(data, len);
	else if (status_ = SB_STATUS_MSG)
		OnMsgData(data, len);
}

void CMsnSwitchBoard::OnSend()
{

}

void CMsnSwitchBoard::OnTimeout()
{

}

void CMsnSwitchBoard::OnClose()
{
	if (!is_quiting_)
	{
		acl_assert(ns_client_);
		ns_client_->DeleteSwitchBorard(this);
	}
	delete this;
	printf(">>>CMsnSwitchBoard: delete now\r\n");
}

// 聊天命令入口

void CMsnSwitchBoard::OnCmd(char* data, int dlen)
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

	int   argc = args->argc;

	if (acl_alldig(args->argv[0]))
		curr_seq_ = atoi(args->argv[0]);

	char** argv = args->argv;

	if (strcasecmp(cmd, "ANS") == 0)
		OnCmdANS(argc, argv, ptr);
	else if (strcasecmp(cmd, "IRO") == 0)
		OnCmdIRO(argc, argv, ptr);
	else if (strcasecmp(cmd, "ACK") == 0)
		OnCmdACK(argc, argv, ptr);
	else if (strcasecmp(cmd, "NAK") == 0)
		OnCmdNAK(argc, argv, ptr);
	else if (strcasecmp(cmd, "USR") == 0)
		OnCmdUSR(argc, argv, ptr);
	else if (strcasecmp(cmd, "MSG") == 0)
		OnCmdMSG(argc, argv, ptr);
	else if (strcasecmp(cmd, "UBM") == 0)
		OnCmdUBM(argc, argv, ptr);
	else if (strcasecmp(cmd, "JOI") ==0)
		OnCmdJOI(argc, argv, ptr);
	else if (strcasecmp(cmd, "BYE") ==0)
		OnCmdBYE(argc, argv, ptr);
	else if (strcasecmp(cmd, "OUT") ==0)
		OnCmdOUT(argc, argv, ptr);

	acl_argv_free(args);
}

// ANS: ANSWER
void CMsnSwitchBoard::OnCmdANS(int argc, char** argv, const char* src)
{
	ready_ = true;
}

// [IRO] 1 1 2 zhengshuxin@hotmail.com;{0a68b3b4-27a9-46f4-a98e-05ffd16d684d} zhengshuxin@hotmail.com 0:0
void CMsnSwitchBoard::OnCmdIRO(int argc, char** argv, const char* src)
{
	if (argc < 6)
	{
		logger_error("argc(%d) invalid, src: %s", argc, src);
		return;
	}
	total_users_ = atoi(argv[2]);

	char* user = argv[3];
	char* ptr = strchr(user, ';');
	if (ptr)
		*ptr = 0;
	AddUser(user, argv[4]);
}

void CMsnSwitchBoard::OnCmdACK(int argc, char** argv, const char* src)
{

}

void CMsnSwitchBoard::OnCmdNAK(int argc, char** argv, const char* src)
{

}

// [USR] 1 OK forward_day@hotmail.com;{12fc37fa-52cc-6c02-7f02-275a70a17a63} xxxx
void CMsnSwitchBoard::OnCmdUSR(int argc, char** argv, const char* src)
{
	if (argc < 4)
	{
		logger_error("argc(%d) invalid, src: %s", argc, src);
		return;
	}
	else if (strcasecmp(argv[1], "OK") != 0)
	{
		logger_error("argv[1]: %s invalid, src: %s", argv[1], src);
		return;
	}

	acl::string msg;
	msg.format("CAL %d %s\r\n", id_++, peer_user_.c_str());
	Send(msg.c_str(), msg.length());
}

// [MSG] zhengshuxin@hotmail.com zhengshuxin@hotmail.com 132
void CMsnSwitchBoard::OnCmdMSG(int argc, char** argv, const char* src)
{
	if (argc < 3)
	{
		logger_error("argc(%d) invalid, src: %s", argc, src);
		return;
	}

	msg_length_ = atoi(argv[2]);
	if (msg_length_ <= 0)
	{
		logger_error("msg_length: %d invalid, src: %s", msg_length_, src);
		return;
	}

	// 设置当前 IO 状态，以便于在 OnRead 中能切换到 OnMsgData 过程
	status_ = SB_STATUS_MSG;

	msg_read_ = 0;  // 记录已读数据的长度

	// 读取 MSG 的数据体
	Read(msg_length_);
}

void CMsnSwitchBoard::OnCmdUBM(int argc, char** argv, const char* src)
{

}

// JOI: JOIN
// [JOI] forward_day@hotmail.com sss 2416181284:2147619840
// [JOI] forward_day@hotmail.com 郑树新--视频加速器：http://v.hexun.com/DownloadNew.htm 2416181284:2147619840
void CMsnSwitchBoard::OnCmdJOI(int argc, char** argv, const char* src)
{
	if (argc < 3)
	{
		logger_error("argc(%d) invalid, src: %s", argc, src);
		return;
	}

	char* passport = argv[0];
	char* display_name = argv[1];
	AddUser(passport, display_name);

	if (is_invited_)
		return;

	std::list<CMsnMsg*>::iterator it_next, it = chat_msgs_.begin();
	for (; it != chat_msgs_.end(); it = it_next)
	{
		it_next = it;
		it_next++;
		SendMsg(*it);
		delete (*it);
		chat_msgs_.erase(it);
	}
}

// 对方离开时的命令
void CMsnSwitchBoard::OnCmdBYE(int argc, char** argv, const char* src)
{

}

// 对方退出时的命令
void CMsnSwitchBoard::OnCmdOUT(int argc, char** argv, const char* src)
{

}

/*
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
X-MMS-IM-Format: FN=Microsoft%20YaHei%20; EF=;CO=0; CS=1; PF=0
*/
void CMsnSwitchBoard::OnMsgData(char* data, int len)
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
			len -= (int) (pdata - plast);
			mime_.update("\r\n", 2);
			break;
		}

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

		if (strcasecmp(ptr, "text/x-msmsgscontrol") == 0)
		{
			// 聊天状态命令
			OnMsgControl(argc, argv);
			break;
		}
		else if (strcasecmp(ptr, "text/plain") == 0)
		{
			// 文本聊天内容
			OnMsgText(argc, argv, pdata, len);
			break;
		}
	}

	// 切换至命令状态
	status_ = SB_STATUS_CMD;
	Gets();
}

void CMsnSwitchBoard::OnMsgControl(int argc, const char** argv)
{
	const char* ptr = mime_.header_value("TypingUser");
	if (ptr)
		printf("%s: is typing\r\n", ptr);
}

void CMsnSwitchBoard::OnMsgText(int argc, const char** argv,
	const char* data, int len)
{
	if (data == NULL || len <= 0)
		return;

	acl::string buf;

	conv_.convert("UTF-8", "GBK", data, len, &buf);
	printf("%s: %s\r\n", peer_user_.c_str(), buf.c_str());

	//buf.format("ACK %d\r\n", curr_seq_);
	//Send(buf.c_str(), buf.length());
}