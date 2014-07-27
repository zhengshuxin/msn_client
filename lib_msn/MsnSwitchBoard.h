#pragma once
#include <time.h>
#include "string.hpp"
#include "mime.hpp"
#include "charset_conv.hpp"
#include "MsnChat.h"
#include "lib_msn.h"

class CMsnNSClient;

/**
* 聊天消息对象
*/
class LIB_MSN_API CMsnMsg
{
public:
	CMsnMsg(const char* data, size_t dlen)
	{
		data_.copy(data, dlen);
		stamp_ = time(NULL);
	}

	~CMsnMsg()
	{

	}

	const acl::string& GetData() const
	{
		return (data_);
	}

	time_t GetStamp() const
	{
		return (stamp_);
	}
private:
	time_t stamp_;
	acl::string data_;
};

//////////////////////////////////////////////////////////////////////////

// 该对象用于与MSN的消息服务器对接，当收到来自于客户的消息时便
// 会启动该对象，将它设计为一个阻塞过程，放在一个单独的线程中
// 运行，其与主线程之间采用IPC通信方式
// 所有的 CMsnSwitchBoard 对象都必须从属于某一个 CMsnNSClient 对象，
// 只有 CMsnNSClient 对象保持存活时 CMsnSwitchBoard 对象才能存活

class LIB_MSN_API CMsnSwitchBoard : public CMsnChat
{
public:
	// 构造函数
	CMsnSwitchBoard(CMsnNSClient* ns_client, const char* peer_user);

	// 析构函数
	~CMsnSwitchBoard(void);

	// 已经连接成功，则直接发送
	void SendMsg(const char* data);
	void AppendMsg(const char* data);
	void SendMsg(CMsnMsg* msg);

	// 设置会话认证的 KEY
	void SetAuthKey(const char* auth_key);

	/**
	* 设置会话 ID
	* @param sid {const char*}
	*/
	void SetSid(const char* sid);

	/**
	* 是否设置为被邀请方式
	* @param yesno {bool} 如果为 true 则设置为被动邀请方式，
	*  否则为主动邀请方式
	* 注：如果不调用本函数设置邀请方式，则默认为主动邀请方式
	*/
	void SetInvited(bool yesno = true);

	/**
	* 判断是否被邀请方式
	* @return {bool}
	*/
	bool IsInvited(void) const;

	bool IsConnected(void) const
	{
		return (is_connected_);
	}

	void SetXfrId(int xfr_id)
	{
		xfr_id_ = xfr_id;
	}

	int GetXfrId() const
	{
		return (xfr_id_);
	}

	const acl::string& GetPeerUser() const
	{
		return (peer_user_);
	}

	void Quit();

	// 添加聊天对象
	void AddUser(const acl::string& user_in,
		const acl::string& display_name);
protected:
	virtual void OnOpen();
	virtual void OnRead(char* data, int len);
	virtual void OnSend();
	virtual void OnTimeout();
	virtual void OnClose();
private:
	acl::string peer_user_;
	acl::string sid_;
	acl::string auth_key_;
	acl::string server_addr_;
	CMsnNSClient* ns_client_;
	bool is_invited_;
	int id_;
	int xfr_id_;
	int curr_seq_;
	bool ready_;
	bool is_connected_;
	bool is_quiting_;

	int status_;
#define  SB_STATUS_CMD	0
#define  SB_STATUS_MSG  1

	int total_users_;
	int current_users_;

	acl::charset_conv conv_;
	acl::mime mime_;
	int  msg_length_;
	int  msg_read_;

	// 聊天消息集合
	std::list<CMsnMsg*> chat_msgs_;

	// 当前的聊天对象列表
	std::map<acl::string, acl::string> chat_users_;

	// 开始会议过程
	void Start();

	void OnCmd(char* data, int dlen);
	void OnCmdANS(int argc, char** argv, const char* src);
	void OnCmdIRO(int argc, char** argv, const char* src);
	void OnCmdACK(int argc, char** argv, const char* src);
	void OnCmdNAK(int argc, char** argv, const char* src);
	void OnCmdUSR(int argc, char** argv, const char* src);
	void OnCmdMSG(int argc, char** argv, const char* src);
	void OnCmdUBM(int argc, char** argv, const char* src);
	void OnCmdJOI(int argc, char** argv, const char* src);
	void OnCmdBYE(int argc, char** argv, const char* src);
	void OnCmdOUT(int argc, char** argv, const char* src);

	void OnMsgData(char* data, int len);
	void OnMsgControl(int argc, const char** argv);
	void OnMsgText(int argc, const char** argv, const char* data, int len);
};
