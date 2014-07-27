#pragma once
#include "mime.hpp"
#include "string.hpp"
#include "xml.hpp"
#include "MsnPassport.h"
#include "MsnContactManager.h"
#include "msnchat.h"
#include "lib_msn.h"

class CMsnClient;
class CMsnTicket;
class CMsnOim;
class CMsnSwitchBoard;

// 通知服务器 (Notification Server, NS)
class LIB_MSN_API CMsnNSClient : public CMsnChat
{
public:
	CMsnNSClient(const char* account, CMsnClient* client,
		const char* display_name = NULL);
	~CMsnNSClient(void);

	void Login(const CMsnTicket* ticket);

	int GetVersion() const
	{
		return (protocol_ver_);
	}

	const acl::string& GetAccount() const
	{
		return (account_);
	}

	CMsnContactManager& GetContactManager()
	{
		return (contactManager_);
	}

	void DeleteSwitchBorard(CMsnSwitchBoard* sb);
	void SpeakTo(const char* toUser, const char* data, size_t dlen);

	// 获得联系人过程完毕后的回调函数
	void OnGetContactFinish(bool ok);
protected:
	// CMsnChat 类虚接口
	virtual void OnOpen();
	virtual void OnRead(char* data, int len);
	virtual void OnSend();
	virtual void OnTimeout();
	virtual void OnClose();
private:
	acl::charset_conv conv_;
	CMsnClient* client_;
	acl::string account_;
	acl::string display_name_;
	acl::xml gcf_body_;
	acl::string mbi_key_;
	acl::string nonce_;
	acl::string remote_user_;
	int  curr_seq_;
	int  gcf_length_;
	int  gcf_read_;
	int  msg_length_;
	int  msg_read_;
	int  status_;
#define NS_STATUS_CMD	0
#define NS_STATUS_GCF	1
#define NS_STATUS_MSG	2
#define NS_STATUS_UBX	3
	const CMsnTicket* ticket_;
	acl::mime mime_;
	CMsnPassport passport_;
	CMsnContactManager contactManager_;
	CMsnOim* oim_;  // 离线消息

	// 聊天面板集合
	std::map<acl::string, CMsnSwitchBoard*> sb_list_;

	bool logged_in_;
	int  adl_fqy_;
	int  protocol_ver_;
private:
	void OnCmd(char* data, int len);
	void OnCmdVER(int argc, const char** argv, const char* src);
	void OnCmdCVR(int argc, const char** argv, const char* src);
	void OnCmdGCF(int argc, const char** argv, const char* src);
	void OnCmdUSR(int argc, const char** argv, const char* src);
	void OnCmdCHL(int argc, const char** argv, const char* src);
	void OnCmdSBS(int argc, const char** argv, const char* src);
	void OnCmdMSG(int argc, const char** argv, const char* src);
	void OnCmdUBX(int argc, const char** argv, const char* src);
	void OnCmdQRY(int argc, const char** argv, const char* src);
	void OnCmdADL(int argc, const char** argv, const char* src);
	void OnCmdBLP(int argc, const char** argv, const char* src);
	void OnCmdPRP(int argc, const char** argv, const char* src);
	void OnCmdCHG(int argc, const char** argv, const char* src);
	void OnCmdILN(int argc, const char** argv, const char* src);
	void OnCmdXFR(int argc, const char** argv, const char* src);
	void OnCmdNLN(int argc, const char** argv, const char* src);
	void OnCmdRNG(int argc, const char** argv, const char* src);
	void OnCmdFLN(int argc, const char** argv, const char* src);

	void OnGcfData(char* data, int len);
	void OnMsgData(char* data, int len);
	void OnUbxData(char* data, int len);

	void OnMsgProfile(int argc, const char** argv);
	void OnMsgInitialMdata(int argc, const char** argv, const char* ptr, size_t n);
	void OnMsgInitialEmail(int argc, const char** argv);
	void OnMsgEmail(int argc, const char** argv);
	void OnMsgDelOim(int argc, const char** argv);
	void OnMsnMsgSystem(int argc, const char** argv);
	void OnMsnMsgPlain(int argc, const char** argv);
	void OnMsnMsgControl(int argc, const char** argv);
	void OnMsnMsgDataCast(int argc, const char** argv);

	//////////////////////////////////////////////////////////////////////////
	void SendPrivacy();
	void DumpContact();
	void SetPublicAlias();

	//////////////////////////////////////////////////////////////////////////
	void FinishLogin();

	// 根据对方帐号获得一个会话板
	CMsnSwitchBoard* GetSwitchBoard(const char* peer_user);
	CMsnSwitchBoard* GetSwitchBoard(int xfr_id);
};
