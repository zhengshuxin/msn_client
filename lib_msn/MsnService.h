#pragma once

class CMsnService : public acl::ipc_service
{
public:
	CMsnService(void);
	~CMsnService(void);

	// 进行 SSO 登入过程
	void start_sso(CMsnClient* client, const char* account,
		const char* passwd);

	// 获得联系人过程
	void start_get_contact(CMsnNSClient* ns_client, const CMsnTicket& ticket);
private:
	void win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
