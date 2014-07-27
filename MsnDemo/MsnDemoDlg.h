// MsnDemoDlg.h : 头文件
//

#pragma once

#include "MsnClient.h"
#include "lib_msn.h"

// CMsnDemoDlg 对话框
class CMsnDemoDlg : public CDialog, public IMsnClient
{
// 构造
public:
	CMsnDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

	~CMsnDemoDlg();				// 标准析构函数

// 对话框数据
	enum { IDD = IDD_MSNDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLogin();

private:
	CMsnClient* msn_client_;

	bool GetLoginInfo(CString& account, CString& passwd);
	bool GetChatInfo(CString& toUser, CString& data);
public:
	afx_msg void OnBnClickedSend();
};
