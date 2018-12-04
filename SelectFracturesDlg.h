#pragma once
#include "Utils.h"
#include "afxcmn.h"


// CSelectFracturesDlg 对话框

class CSelectFracturesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectFracturesDlg)

public:
	CSelectFracturesDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelectFracturesDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHOOSE_FRAGMENT_AND_FRACTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	static int firstPickedFragmentId;
	bool isSelected = false;
	bool isFirstPicked = true;
	int isRender = 0;
	int fragmentId, fractureId;
	Utils utils;
	string rootDir;
	string prefix;
	vector<string> fragmentNames;
	vector<vector<string>> fractureNames;
	vector<vector<int>> visited;

	vtkSmartPointer<vtkRenderWindow> renderWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
public:
	CTreeCtrl m_treeFragmentsFractures;
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreeFragmentsAndFractures(NMHDR *pNMHDR, LRESULT *pResult);

	void SetNames(string rootDir, vector<string> fragmentNames, vector<vector<string>> fractureNames);
	void SetVisited(vector<vector<int>> visited);
	void SetFixedFractureId(bool flag);
	vector<int> GetId();
	afx_msg void OnNMDblclkTreeFragmentsAndFractures(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSelectConfirm();
	afx_msg void OnNMClickTreeFragmentsAndFractures(NMHDR *pNMHDR, LRESULT *pResult);
};
