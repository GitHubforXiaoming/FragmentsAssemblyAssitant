#pragma once

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <string>
#include <vector>

#include <direct.h>
#include <io.h>

using namespace std;


const float COLOR_RAINBOW[10][3] = {
	{ 236, 247, 82 },
	{ 121, 232, 208 },
	{ 255, 184, 184 },
	{ 134, 230, 90 },
	{ 36, 209, 151 },
	{ 150, 184, 255 },
	{ 216, 99, 242 },
	{ 255, 241, 153 },
	{ 173, 255, 153 },
	{ 255, 148, 148 }
};

const float COLOR_BACKGROUND_LIGHT[5][3] = {
	{ 239, 206, 232 },
	{ 243, 215, 181 },
	{ 253, 255, 233 },
	{ 218, 249, 202 },
	{ 199, 179, 229 }
};


class Utils
{
private:
	string axis[3];
	string message[13];
public:
	Utils();
	~Utils();
	static vector<string> SplitString(const string str, const string pattern);
	
	void GetAllFormatFiles(string path, vector<string>& files, string format);

	void GetAllFormatFilesWithDirectory(string path, vector<string>& files, string format);

	/********************************
	* 函数名：ReadDatas

	* 描述：根据指定的文件路径读取（.stl）文件

	* 作者：小明

	* 日期：2018-7-4 15:15:33

	********************************/
	vector<vtkSmartPointer<vtkPolyData>> ReadDatas(string modelDir, vector<string>& files);

	vector<vector<vtkSmartPointer<vtkPolyData>>> ReadFractures(string modelDir, vector<vector<string>>& files);

	vector<vector<vtkSmartPointer<vtkPolyData>>> ReadFracturesWithPrefix(string modelDir, string& fracturesDir, vector<vector<string>>& files, string& prefix);


	/********************************
	* 函数名：ReadOneData

	* 描述：根据模型的名称读取stl文件

	* 作者：小明

	* 日期：2018-7-4 11:07:06

	********************************/
	vtkSmartPointer<vtkPolyData> ReadOneData(string name);


	static string UnicodeToANSI(const CString &str);


	string LOG(int op, string value);

	static wstring StringToWString(string& str);
};

