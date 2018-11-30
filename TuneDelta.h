#pragma once


// CTuneDelta 对话框

class CTuneDelta : public CDialogEx
{
	DECLARE_DYNAMIC(CTuneDelta)

public:
	CTuneDelta(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTuneDelta();

// 对话框数据
	enum { IDD = IDD_DIALOG_DELTA_CONF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_editValueOfDelta;
	afx_msg void OnBnClickedButtonConfConfirm();
	double GetDelta();
};
