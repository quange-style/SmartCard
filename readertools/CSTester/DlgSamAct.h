#pragma once


// CDlgSamAct �Ի���

class CDlgSamAct : public CDialog
{
	DECLARE_DYNAMIC(CDlgSamAct)

public:
	CDlgSamAct(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSamAct();

// �Ի�������
	enum { IDD =  IDD_DLG_SAM_ACT};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnSamAct();
	afx_msg void OnBnClickedBnCancel();
	virtual BOOL OnInitDialog();
};
