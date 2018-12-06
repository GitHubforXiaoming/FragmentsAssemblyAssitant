
// FragmentsRestorationAssitantDlg.h : 头文件
//

#pragma once
#include "Registration.h"
#include "Utils.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "TuneDelta.h"
#include "SelectFracturesDlg.h"

// CFragmentsRestorationAssitantDlg 对话框
class CFragmentsRestorationAssitantDlg : public CDialogEx
{
private:
	static CString defaultPath;
	string curSelectedFractures[2];
	string matchPairFileName;
	int AXIS = 0;
	double THETA_DELTA = 0.5;
	double DISTANCE_DELTA = 0.1;
	bool IS_TUNE = false;
	CString dir;
	string prefix;
	string fracturesDir;
	Utils utils;
	Registration registration;

	vector<string> fragmentNames;
	vector<vector<string>> fractureNames;
	vector<vtkSmartPointer<vtkPolyData>> fragments;
	vector<vector<vtkSmartPointer<vtkPolyData>>> fracturess;
	vector<vector<int>> visited;

	double* rotateAxis;
	double fixedCenter[3], floatCenter[3];
	double majorAxis[3], minorAxis[3], axis[3]; // 分别表示固定断面的长轴短轴和两个轴的向量积
	vtkSmartPointer<vtkPolyData> fixedFracture;
	vtkSmartPointer<vtkPolyData> floatFracture;
	vtkSmartPointer<vtkPolyData> fixedFragment;
	vtkSmartPointer<vtkPolyData> floatFragment;
	vector<pair<vtkSmartPointer<vtkPolyData>, string>> floatFractures;	// 在同一碎片上的断面
	vector<vtkSmartPointer<vtkPolyData>> transformedDatas;

	vector<vtkSmartPointer<vtkPolyData>> originalPairOfFragments;
	vector<vtkSmartPointer<vtkPolyData>> pairOfFragments;
	vector<vtkSmartPointer<vtkPolyData>> pairOfFractures;

	vtkSmartPointer<vtkRenderWindow> renderWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;

	vtkSmartPointer<vtkRenderWindow> originalRendererWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> originalInteractor;

	vtkSmartPointer<vtkRenderWindow> adjustedRendererWindow;
	vtkSmartPointer<vtkRenderWindowInteractor> adjustedInteractor;


// 构造
public:
	CFragmentsRestorationAssitantDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FRAGMENTSRESTORATIONASSITANT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HICON m_btnRoateConfIcon;
	HICON m_btnTranslateConfIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	static	int CALLBACK  BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

	void InitFragmentsView();

	void RecordOperation(int op, string value);

	/********************************
	* 函数名：DyeForPoint

	* 描述：为点集染色

	* 作者：小明

	* 日期：2018-4-10 21:15:29

	********************************/
	vtkSmartPointer<vtkUnsignedCharArray> DyeForPoint(int numOfPoints, int scheme);

	/********************************
	* 函数名：DrawPCA

	* 描述：绘制模型的主方向

	* 作者：小明

	* 日期：2018-4-9 08:32:42

	********************************/
	vtkSmartPointer<vtkPolyData> DrawPCA(vtkSmartPointer<vtkPolyData> frature, vector<vtkSmartPointer<vtkLineSource>>& lines, double* maxEigenVector, double* secEigenVector, double center[3]);

	/********************************
	* 函数名：PreRegistration

	* 描述：预配准,返回预配准旋转矩阵

	* 作者：小明

	* 日期：2018-4-10 10:47:21

	********************************/
	vector<vtkSmartPointer<vtkPolyData>> CFragmentsRestorationAssitantDlg::PreRegistration(vtkSmartPointer<vtkPolyData> fracture1, vtkSmartPointer<vtkPolyData> fracture2, vtkSmartPointer<vtkMatrix4x4>& matrix);

	/********************************
	* 函数名：TransformFracturesOnFragment

	* 描述：变换碎片上的断面

	* 作者：小明

	* 日期：2018-7-19 09:12:48

	********************************/
	void TransformFracturesOnFragment(vtkSmartPointer<vtkPolyData>& fragment, vector<vtkSmartPointer<vtkPolyData>>& fractures, vtkSmartPointer<vtkMatrix4x4> matrix);

	void TransformFracturesOnTheSameFragment(vector<pair<vtkSmartPointer<vtkPolyData>, string>>& fractures, vtkSmartPointer<vtkMatrix4x4> matrix);

	void TransformDatas(vtkSmartPointer<vtkPolyData>& data, vtkSmartPointer<vtkMatrix4x4> matrix);

	void TransformDatas(vector<vtkSmartPointer<vtkPolyData>>& datas, vtkSmartPointer<vtkMatrix4x4> matrix);

	void TransformCenter(double* center, vtkSmartPointer<vtkMatrix4x4> matrix);

	void Translate(vtkSmartPointer<vtkMatrix4x4>& matrix, double* axis, double distance);

	void RotateAndTranslate(vtkSmartPointer<vtkMatrix4x4>& matrix, double* axis, double theta);

	/********************************
	* 函数名：WriteModel

	* 描述：将模型写入文件中

	* 作者：小明

	* 日期：2018-2-3 18:40:04

	********************************/
	void WriteModel(vtkSmartPointer<vtkPolyData> data, string path);

	void Display(int itemId, vtkSmartPointer<vtkRenderWindow> renWin, vtkSmartPointer<vtkRenderWindowInteractor> interactor);
	void DisplayFragmentsAndFracturess(vector<vtkSmartPointer<vtkPolyData>> fragments, vector<vector<vtkSmartPointer<vtkPolyData>>> fracturess, vtkSmartPointer<vtkRenderWindow>& renderWindow);
	void DisplayFragmentsAndFractures(vector<vtkSmartPointer<vtkPolyData>> fragments, vector<vtkSmartPointer<vtkPolyData>> fractures, vtkSmartPointer<vtkRenderWindow>& renderWindow, vtkSmartPointer<vtkMatrix4x4> matrix);
	void DisplayMoreModels(vector<vtkSmartPointer<vtkPolyData>> datas, vtkSmartPointer<vtkRenderWindow>& renderWindow);
	void DisplayModel(vtkSmartPointer<vtkPolyData> data, vtkSmartPointer<vtkRenderWindow>& renderWindow);

	int m_radioMajorAxis;
	CString m_csModelDir;
	CEdit m_eModelsDir;
	CListCtrl m_lFragmentsList;
	CSpinButtonCtrl m_spinTurnRotate;
	CSpinButtonCtrl m_spinTurnTranslate;
	CString m_editValueOfRotate;
	CString m_editValueOfTranslate;
	CEdit m_editRecordOperation;
	CString m_editValueOfRecordOperation;
	CButton m_btnRotateConf;
	CButton m_btnTranslateConf;
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedButtonChooseFixFracture();
	afx_msg void OnBnClickedButtonChooseFloatFracture();
	afx_msg void OnBnClickedButtonFirstPca();
	afx_msg void OnBnClickedButtonTurnOver();
	afx_msg void OnBnClickedRadioMajorAxis();
	afx_msg void OnBnClickedButtonOpenFragmentsAndFractures();
	afx_msg void OnDeltaposSpinTurnRotate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinTurnTranslate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRotateConf();
	afx_msg void OnBnClickedButtonTranslateConf();
	afx_msg void OnBnClickedButtonSaveFragment();
	afx_msg void OnBnClickedButtonSecondPca();
	afx_msg void OnBnClickedButtonRechose();
	afx_msg void OnBnClickedButtonSaveRelation();
};
