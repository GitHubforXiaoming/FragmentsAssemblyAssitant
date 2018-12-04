#include "stdafx.h"
#include "Utils.h"


Utils::Utils()
{
	axis[0] = "主轴";
	axis[1] = "次轴";
	axis[2] = "z轴";

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == 0)
				message[i * 3 + j] = "绕" + axis[j] + "翻转";
			else if (i == 1)
				message[i * 3 + j] = "绕" + axis[j] + "旋转";
			else
				message[i * 3 + j] = "沿" + axis[j] + "平移";
		}
	}
	
	message[9] = "载入文件";
	message[10] = "加载固定部分";
	message[11] = "加载浮动部分";
	message[12] = "PCA预配准";
}


Utils::~Utils()
{
}

vector<string> Utils::SplitString(const string str, const string pattern)
{
	vector<string> splitted;
	if ("" == str)
	{
		return splitted;
	}

	std::string strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();

	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		splitted.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}
	return splitted;
}

void Utils::GetAllFormatFiles(string path, vector<string>& files, string format)
{
	//文件句柄      
	long   hFile = 0;
	//文件信息      
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(p.assign(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

void Utils::GetAllFormatFilesWithDirectory(string path, vector<string>& files, string format)
{
	//文件句柄      
	long   hFile = 0;
	//文件信息      
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(path + p.assign(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

string Utils::UnicodeToANSI(const CString &str)
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		str.GetString(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP,
		0,
		str.GetString(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}


vector<vtkSmartPointer<vtkPolyData>> Utils::ReadDatas(string modelDir, vector<string>& fileNames)
{
	vector<vtkSmartPointer<vtkPolyData>> datas;
	vector<string> names;
	GetAllFormatFiles(modelDir, names, ".stl");
	for (unsigned int i = 0; i < names.size(); i++)
	{
		vtkSmartPointer<vtkPolyData> data = ReadOneData(modelDir + names[i]);
		datas.push_back(data);
	}
	fileNames = names;
	return datas;
}

vector<vector<vtkSmartPointer<vtkPolyData>>> Utils::ReadFractures(string modelDir, vector<vector<string>>& files)
{
	vector<vector<vtkSmartPointer<vtkPolyData>>> fracturess;
	
	vector<vtkSmartPointer<vtkPolyData>> datas;
	vector<string> names;
	GetAllFormatFiles(modelDir, names, ".stl");
	for (unsigned int i = 0; i < names.size(); i++)
	{
		vector<string> fileNames;
		vector<vtkSmartPointer<vtkPolyData>> fractures= ReadDatas(modelDir + "fragment-" + to_string(i + 1) + "-fracture-surfaces\\", fileNames);
		for (unsigned int j = 0; j < fileNames.size(); j++)
		{
			fileNames[j] = "fragment-" + to_string(i + 1) + "-fracture-surfaces\\" + fileNames[j];
		}
		files.push_back(fileNames);
		fracturess.push_back(fractures);
	}

	return fracturess;
}


vector<vector<vtkSmartPointer<vtkPolyData>>> Utils::ReadFracturesWithPrefix(string modelDir, string& fracturesDir, vector<vector<string>>& files, string& prefix)
{
	vector<string> splitted = SplitString(modelDir, "\\");
	prefix = splitted[splitted.size() - 2];
	for (unsigned int i = 0; i < splitted.size() - 3; i++)
	{
		fracturesDir += splitted[i] + "\\";
	}
	fracturesDir += "fractures\\";
	prefix = prefix.substr(6, prefix.length());
	vector<vtkSmartPointer<vtkPolyData>> datas;
	vector<string> fragmentNames;
	vector<string> fractureNames;
	GetAllFormatFiles(modelDir, fragmentNames, ".stl");
	GetAllFormatFiles(fracturesDir, fractureNames, ".stl");
	files.resize(fragmentNames.size());
	vector<vector<vtkSmartPointer<vtkPolyData>>> fracturess(fragmentNames.size());
	for (unsigned int i = 0; i < fractureNames.size(); i++)
	{
		vector<string> fileNames;
		vector<string> s = SplitString(fractureNames[i], "-");
		if (s[0] == prefix)
		{

			vtkSmartPointer<vtkPolyData> fracture = ReadOneData(fracturesDir + fractureNames[i]);
			files[atoi(s[1].c_str()) - 1].push_back( fracturesDir + fractureNames[i]);
			fracturess[atoi(s[1].c_str()) - 1].push_back(fracture);
		}
	}

	return fracturess;
}



vtkSmartPointer<vtkPolyData> Utils::ReadOneData(string name)
{
	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(name.c_str());
	reader->Update();
	return reader->GetOutput();
}

string Utils::LOG(int op, string value)
{
	if (op < 3 || op > 8)
		return message[op];
	else
		return message[op] + value;
}

wstring Utils::StringToWString(string& str)
{
	wstring wstr(str.length(), L' ');
	copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}
