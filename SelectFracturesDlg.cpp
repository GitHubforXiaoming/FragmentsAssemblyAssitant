// SelectFracturesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FragmentsRestorationAssitant.h"
#include "SelectFracturesDlg.h"
#include "afxdialogex.h"


// CSelectFracturesDlg 对话框

IMPLEMENT_DYNAMIC(CSelectFracturesDlg, CDialogEx)

CSelectFracturesDlg::CSelectFracturesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectFracturesDlg::IDD, pParent)
{
	fragmentId = 0;
	fractureId = 0;
	renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
}


CSelectFracturesDlg::~CSelectFracturesDlg()
{
}

void CSelectFracturesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FRAGMENTS_AND_FRACTURES, m_treeFragmentsFractures);
}


BEGIN_MESSAGE_MAP(CSelectFracturesDlg, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FRAGMENTS_AND_FRACTURES, &CSelectFracturesDlg::OnTvnSelchangedTreeFragmentsAndFractures)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_FRAGMENTS_AND_FRACTURES, &CSelectFracturesDlg::OnNMDblclkTreeFragmentsAndFractures)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CONFIRM, &CSelectFracturesDlg::OnBnClickedButtonSelectConfirm)
	ON_NOTIFY(NM_CLICK, IDC_TREE_FRAGMENTS_AND_FRACTURES, &CSelectFracturesDlg::OnNMClickTreeFragmentsAndFractures)
END_MESSAGE_MAP()

int CSelectFracturesDlg::firstPickedFragmentId;

// CSelectFracturesDlg 消息处理程序


BOOL CSelectFracturesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	HTREEITEM hRoot;     // 树的根节点的句柄   
	HTREEITEM hFragmentItem; // 可表示任一分类节点的句柄   
	HTREEITEM hFractureItem;  // 可表示任一文章节点的句柄   
	// 插入根节点   
	hRoot = m_treeFragmentsFractures.InsertItem(CStringW(rootDir.c_str()), 0, 0);
	// 在根节点下插入子节点
	int n = 0; 
	for (unsigned int i = 0; i < fragmentNames.size(); i++)
	{
		hFragmentItem = m_treeFragmentsFractures.InsertItem(CStringW(fragmentNames[i].c_str()), 1, 1, hRoot, TVI_LAST);
		m_treeFragmentsFractures.SetItemData(hFragmentItem, (i + 1) * 10);
		for (unsigned int j = 0; j < fractureNames[i].size(); j++)
		{
			string fractureName = fractureNames[i][j];
			hFractureItem = m_treeFragmentsFractures.InsertItem(CStringW(fractureName.substr(fractureName.length() - 5, fractureName.length()).c_str()),
				2, 2, hFragmentItem, TVI_LAST);
			m_treeFragmentsFractures.SetItemData(hFractureItem, (i + 1) * 10 + (j + 1));
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CSelectFracturesDlg::OnTvnSelchangedTreeFragmentsAndFractures(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


void CSelectFracturesDlg::SetNames(string rootDir, vector<string> fragmentNames, vector<vector<string>> fractureNames)
{
	this->rootDir = rootDir;
	this->fragmentNames = fragmentNames;
	this->fractureNames = fractureNames;
}


void CSelectFracturesDlg::SetVisited(vector<vector<int>> visited)
{
	this->visited = visited;
}

void CSelectFracturesDlg::SetFixedFractureId(bool flag)
{
	isFirstPicked = flag;
}

vector<int> CSelectFracturesDlg::GetId()
{
	vector<int> index;
	index.push_back(this->fragmentId - 1);
	index.push_back(this->fractureId - 1);
	return index;
}

void CSelectFracturesDlg::OnNMDblclkTreeFragmentsAndFractures(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	HTREEITEM hTreeItem = m_treeFragmentsFractures.GetSelectedItem();
	string name;
	if (m_treeFragmentsFractures.GetChildItem(hTreeItem) != NULL)
	{
		isSelected = false;
		name = Utils::UnicodeToANSI(m_treeFragmentsFractures.GetItemText(hTreeItem));
	}
	else
	{
		isSelected = true;
		int id = m_treeFragmentsFractures.GetItemData(hTreeItem);
		fragmentId = id / 10;
		fractureId = id % 10;
		name = fractureNames[fragmentId - 1][fractureId - 1];
		if (isFirstPicked)
			firstPickedFragmentId = fragmentId;

		CRect rect;
		GetDlgItem(IDC_STATIC_PREVIEW_SELECTION)->GetWindowRect(rect);
		CWnd* pWnd = GetDlgItem(IDC_STATIC_PREVIEW_SELECTION);
		HWND hwnd = pWnd->GetSafeHwnd();

		vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

		mapper->SetInputData(utils.ReadOneData(name));

		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		int color = rand() % 10;
		actor->GetProperty()->SetColor(COLOR_RAINBOW[color % 10][0] / 255.0, COLOR_RAINBOW[color % 10][1] / 255.0, COLOR_RAINBOW[color % 10][2] / 255.0);
		renderer->SetBackground(255 / 255.0, 255 / 255.0, 255 / 255.0);

		renderWindow->SetParentId(hwnd);
		renderWindow->SetSize(rect.Width(), rect.Height());
		if (isRender == 0)
		{
			renderer->AddActor(actor);
			renderWindow->AddRenderer(renderer);
			renderWindowInteractor->SetRenderWindow(renderWindow);
			renderWindow->Render();
			//renderWindowInteractor->Start();
		}
		else
		{
			isRender++;
			renderWindow->Render();
		}
		if (/*visited[fragmentId - 1][fractureId - 1]*/0)
		{
			MessageBox(_T("该断面已经被选过"), _T("提示"), MB_OK);
		}
		else if (firstPickedFragmentId == fragmentId && !isFirstPicked)
		{
			MessageBox(_T("选择了同一碎片上的断面"), _T("提示"), MB_OK);
		}
		else
		{
			renderWindow->Delete();
			renderWindowInteractor->Delete();
			CDialog::OnOK();
		}
	}
	*pResult = 0;
	
}


void CSelectFracturesDlg::OnBnClickedButtonSelectConfirm()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!isSelected)
	{
		MessageBox(_T("请选择断面文件"), _T("提示"), MB_OK);
		//CDialog::EndDialog(0);
	}
	if (visited[fragmentId - 1][fractureId - 1])
	{
		MessageBox(_T("该断面已经被选过"), _T("提示"), MB_OK);
	}
	else if (firstPickedFragmentId == fragmentId && !isFirstPicked)
	{
		MessageBox(_T("选择了同一碎片上的断面"), _T("提示"), MB_OK);
	}
	else
	{
		renderWindow->Delete();
		renderWindowInteractor->Delete();
		CDialog::OnOK();
	}
}


void CSelectFracturesDlg::OnNMClickTreeFragmentsAndFractures(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	
}
