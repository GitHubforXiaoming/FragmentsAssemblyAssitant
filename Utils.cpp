#include "stdafx.h"
#include "Utils.h"


Utils::Utils()
{
	axis[0] = "����";
	axis[1] = "����";
	axis[2] = "z��";

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == 0)
				message[i * 3 + j] = "��" + axis[j] + "��ת";
			else if (i == 1)
				message[i * 3 + j] = "��" + axis[j] + "��ת";
			else
				message[i * 3 + j] = "��" + axis[j] + "ƽ��";
		}
	}
	
	message[9] = "�����ļ�";
	message[10] = "���ع̶�����";
	message[11] = "���ظ�������";
	message[12] = "PCAԤ��׼";
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
	//�ļ����      
	long   hFile = 0;
	//�ļ���Ϣ      
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
	//�ļ����      
	long   hFile = 0;
	//�ļ���Ϣ      
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
