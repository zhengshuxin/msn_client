// MsnDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MsnDemo.h"
#include "MsnDemoDlg.h"
#include "MsnGlobal.h"
#include "MsnNSClient.h"
#include ".\msndemodlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMsnDemoDlg 对话框



CMsnDemoDlg::CMsnDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsnDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	msn_client_ = NULL;
}

CMsnDemoDlg::~CMsnDemoDlg()
{
	if (msn_client_)
		delete msn_client_;
}

void CMsnDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMsnDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGIN, OnBnClickedLogin)
	ON_BN_CLICKED(IDC_SEND, OnBnClickedSend)
END_MESSAGE_MAP()


// CMsnDemoDlg 消息处理程序

BOOL CMsnDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MAXIMIZE);

	// TODO: 在此添加额外的初始化代码
	// 初始化 AIO 句柄
	(void) get_aio_handle(true);

	// 打开 DOS 终端
	AllocConsole();
	FILE* fp = freopen("CONOUT$","w+t",stdout);

	GetDlgItem(IDC_USERNAME)->SetWindowText("zhengshuxin@hotmail.com");
	GetDlgItem(IDC_PASSWD)->SetWindowText("zsxNihao");

	GetDlgItem(IDC_TO_USER)->SetWindowText("forward_day@hotmail.com");
	GetDlgItem(IDC_INPUT)->SetWindowText("hello, who're you!");

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CMsnDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMsnDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

bool CMsnDemoDlg::GetLoginInfo(CString& account, CString& passwd)
{
	GetDlgItem(IDC_USERNAME)->GetWindowText(account);
	if (account.GetLength() == 0) {
		MessageBox("帐号为空!");
		return (false);
	}
	GetDlgItem(IDC_PASSWD)->GetWindowText(passwd);
	if (passwd.GetLength() == 0)
	{
		MessageBox("密码为空!");
		return (false);
	}
	return (true);
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CMsnDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMsnDemoDlg::OnBnClickedLogin()
{
	// TODO: 在此添加控件通知处理程序代码

	CString account, passwd;

	if (GetLoginInfo(account, passwd) == false)
		return;

	// 禁止重复登入
	GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);

	msn_client_ = new CMsnClient(this, account.GetString(), passwd.GetString(), 60);
	msn_client_->Login("messenger.hotmail.com", 1863);
}


bool CMsnDemoDlg::GetChatInfo(CString& toUser, CString& data)
{
	GetDlgItem(IDC_TO_USER)->GetWindowText(toUser);
	if (toUser.GetLength() == 0)
	{
		MessageBox("聊天对象为空!");
		return (false);
	}
	GetDlgItem(IDC_INPUT)->GetWindowText(data);
	if (data.GetLength() == 0) {
		MessageBox("内容为空!");
		return (false);
	}

	return (true);
}

void CMsnDemoDlg::OnBnClickedSend()
{
	// TODO: 在此添加控件通知处理程序代码

	if (msn_client_ == NULL)
	{
		MessageBox("请先登入!");
		return;
	}

	CString data, toUser;

	if (GetChatInfo(toUser, data) == false)
		return;
	GetDlgItem(IDC_INPUT)->SetWindowText("");
	CMsnNSClient* ns_client = msn_client_->GetNSClient();
	if (ns_client == NULL)
	{
		MessageBox("You're quit now!");
		return;
	}
	ns_client->SpeakTo(toUser.GetString(), data.GetString(), data.GetLength());
}
