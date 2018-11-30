#include "stdafx.h"
#include "Registration.h"


Registration::Registration()
{

}

Registration::Registration(int fragmentsSize, int fracturesSize)
{
	this->fragmentsSize = fragmentsSize;
	this->fracturesSize = fracturesSize;
	fractures = new int*[fragmentsSize];
	for (int i = 0; i < fragmentsSize; i++)
		fractures[i] = new int[fracturesSize];
	for (int i = 0; i < fragmentsSize; i++)
		for (int j = 0; j < fracturesSize; j++)
			fractures[i][j] = 0;
}

Registration::Registration(vtkSmartPointer<vtkPolyData> source)
{
	this->source = source;
	points = source->GetPoints();
	this->rotateX = vtkSmartPointer<vtkMatrix4x4>::New();
	this->rotateY = vtkSmartPointer<vtkMatrix4x4>::New();
	this->rotateZ = vtkSmartPointer<vtkMatrix4x4>::New();
}

Registration::~Registration()
{
}

double Registration::Norm(double* a)
{
	return sqrt(square(a[0]) + square(a[1]) + square(a[2]));
}

void Registration::Normalize(double* a, double* b)
{
	for (int i = 0; i < 3; i++)
		b[i] = a[i] / Norm(a);
}

double Registration::Multiply(double* a, double* b)
{
	double c = 0;
	for (int i = 0; i < 3; i++)
		c += a[i] * b[i];
	return c;
}

void Registration::GetRotateAxis(double* axis1, double* axis2, double* axis)
{
	axis[0] = axis1[1] * axis2[2] - axis2[1] * axis1[2];
	axis[1] = axis2[0] * axis1[2] - axis1[0] * axis2[2];
	axis[2] = axis1[0] * axis2[1] - axis2[0] * axis1[1];
}

vector<string> Registration::SplitPair(string str, string pattern)
{
	vector<string> split;
	int position = str.find_first_of(pattern.c_str());
	split.push_back(str.substr(0, position));
	split.push_back(str.substr(position + 1, str.length()));
	return split;
}

vector<string> Registration::SplitString(const string str, const string pattern)
{
	vector<string> splitted;
	if ("" == str)
	{
		return splitted;
	}
	//方便截取最后一段数据
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

vector<int> Registration::GetNextFracture(int fragmentId, vector<pair<string, string>> pairs)
{
	vector<int> nextFracture;
	bool over = false;
	for (int i = 0; i < fracturesSize; i++)
	{
		// 寻找当前碎片断面中没有被访问过的断面
		if (fractures[fragmentId][i] == 0)
		{
			// 利用已知的匹配关系寻找在同一碎片上其他断面的下一个匹配断面
			for (vector<pair<string, string>>::iterator it = pairs.begin(); it != pairs.end(); it++)
			{
				int j = atoi(SplitPair(it->first, "-")[0].c_str());
				if (fragmentId == j)
				{
					int m = atoi(SplitPair(it->second, "-")[0].c_str());
					int n = atoi(SplitPair(it->second, "-")[1].c_str());
					nextFracture.push_back(m);
					nextFracture.push_back(n);
					over = true;
					break;
				}
			}
			if (over)
				break;
		}
	}
	return nextFracture;
}

void Registration::MatchStrategy(vector<pair<string, string>> pairs, vector<int>& sequences)
{
	int k = 0;
	int i, j;
	for (vector<pair<string, string>>::iterator it = pairs.begin(); it != pairs.end(); it++)
	{
		if (k == 0)
		{
			i = atoi(SplitPair(it->first, "-")[0].c_str());
			j = atoi(SplitPair(it->first, "-")[1].c_str());
			fractures[i][j] = 1;
			sequences.push_back(i);
		}
		else
		{
			i = GetNextFracture(i, pairs)[0];
			j = GetNextFracture(i, pairs)[1];
			fractures[i][j] = 1;
			sequences.push_back(i);
		}
		k++;
	}
}

vtkSmartPointer<vtkMatrix4x4> Registration::GetRotateX(double theta)
{
	rotateX->SetElement(0, 0, 1);
	rotateX->SetElement(0, 1, 0);
	rotateX->SetElement(0, 2, 0);
	rotateX->SetElement(0, 3, 0);

	rotateX->SetElement(1, 0, 0);
	rotateX->SetElement(1, 1, cos(theta));
	rotateX->SetElement(1, 2, -sin(theta));
	rotateX->SetElement(1, 3, 0);

	rotateX->SetElement(2, 0, 0);
	rotateX->SetElement(2, 1, sin(theta));
	rotateX->SetElement(2, 2, cos(theta));
	rotateX->SetElement(2, 3, 0);

	rotateX->SetElement(3, 0, 0);
	rotateX->SetElement(3, 1, 0);
	rotateX->SetElement(3, 2, 0);
	rotateX->SetElement(3, 3, 1);

	return rotateX;
}

vtkSmartPointer<vtkMatrix4x4> Registration::GetRotateY(double theta)
{
	rotateY->SetElement(0, 0, cos(theta));
	rotateY->SetElement(0, 1, 0);
	rotateY->SetElement(0, 2, sin(theta));
	rotateY->SetElement(0, 3, 0);

	rotateY->SetElement(1, 0, 0);
	rotateY->SetElement(1, 1, 1);
	rotateY->SetElement(1, 2, 0);
	rotateY->SetElement(1, 3, 0);

	rotateY->SetElement(2, 0, -sin(theta));
	rotateY->SetElement(2, 1, 0);
	rotateY->SetElement(2, 2, cos(theta));
	rotateY->SetElement(2, 3, 0);

	rotateY->SetElement(3, 0, 0);
	rotateY->SetElement(3, 1, 0);
	rotateY->SetElement(3, 2, 0);
	rotateY->SetElement(3, 3, 1);

	return rotateY;
}

vtkSmartPointer<vtkMatrix4x4> Registration::GetRotateZ(double theta)
{
	rotateZ->SetElement(0, 0, cos(theta));
	rotateZ->SetElement(0, 1, -sin(theta));
	rotateZ->SetElement(0, 2, 0);
	rotateZ->SetElement(0, 3, 0);

	rotateZ->SetElement(1, 0, sin(theta));
	rotateZ->SetElement(1, 1, cos(theta));
	rotateZ->SetElement(1, 2, 0);
	rotateZ->SetElement(1, 3, 0);

	rotateZ->SetElement(2, 0, 0);
	rotateZ->SetElement(2, 1, 0);
	rotateZ->SetElement(2, 2, 1);
	rotateZ->SetElement(2, 3, 0);

	rotateZ->SetElement(3, 0, 0);
	rotateZ->SetElement(3, 1, 0);
	rotateZ->SetElement(3, 2, 0);
	rotateZ->SetElement(3, 3, 1);

	return rotateZ;
}

void Registration::InstanceMatrix(vtkSmartPointer<vtkMatrix4x4>& matrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
				matrix->SetElement(i, j, 1);
			else
				matrix->SetElement(i, j, 0);
		}
	}
}


void Registration::PCA(vtkSmartPointer<vtkDoubleArray>& eigenvectors)
{
	// These would be all of your "x" values.
	vtkSmartPointer<vtkDoubleArray> xArray =
		vtkSmartPointer<vtkDoubleArray>::New();
	xArray->SetNumberOfComponents(1);
	xArray->SetName("x");

	// These would be all of your "y" values.
	vtkSmartPointer<vtkDoubleArray> yArray =
		vtkSmartPointer<vtkDoubleArray>::New();
	yArray->SetNumberOfComponents(1);
	yArray->SetName("y");

	// These would be all of your "z" values.
	vtkSmartPointer<vtkDoubleArray> zArray =
		vtkSmartPointer<vtkDoubleArray>::New();
	zArray->SetNumberOfComponents(1);
	zArray->SetName("z");

	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
	{
		double p[3];
		points->GetPoint(i, p);
		xArray->InsertNextValue(p[0]);
		yArray->InsertNextValue(p[1]);
		zArray->InsertNextValue(p[2]);
	}

	vtkSmartPointer<vtkTable> datasetTable =
		vtkSmartPointer<vtkTable>::New();
	datasetTable->AddColumn(xArray);
	datasetTable->AddColumn(yArray);
	datasetTable->AddColumn(zArray);

	vtkSmartPointer<vtkPCAStatistics> pcaStatistics =
		vtkSmartPointer<vtkPCAStatistics>::New();
	pcaStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA, datasetTable);

	pcaStatistics->SetColumnStatus("x", 1);
	pcaStatistics->SetColumnStatus("y", 1);
	pcaStatistics->SetColumnStatus("z", 1);

	pcaStatistics->RequestSelectedColumns();
	pcaStatistics->SetDeriveOption(true);
	pcaStatistics->Update();

	///////// Eigenvalues ////////////
	vtkSmartPointer<vtkDoubleArray> eigenvalues =
		vtkSmartPointer<vtkDoubleArray>::New();
	pcaStatistics->GetEigenvalues(eigenvalues);
	/*for (vtkIdType i = 0; i < eigenvalues->GetNumberOfTuples(); i++)
	{
	std::cout << "Eigenvalue " << i << " = " << eigenvalues->GetValue(i) << std::endl;
	}*/

	///////// Eigenvectors ////////////
	pcaStatistics->GetEigenvectors(eigenvectors);
}

void Registration::RotateByAnyAxis(double* start, double* end, vtkSmartPointer<vtkMatrix4x4>& rotateMatrix)
{
	// 计算新的轴，两个向量的合成向量
	double axis[3], rotateAxis[3];
	for (int i = 0; i < 3; i++)
		axis[i] = start[i] + end[i];
	Normalize(axis, rotateAxis);

	// 设定旋转矩阵
	double a = rotateAxis[0];
	double b = rotateAxis[1];
	double c = rotateAxis[2];
	// 第一行
	rotateMatrix->SetElement(0, 0, square(a) + (1 - square(a)) * -1);
	rotateMatrix->SetElement(0, 1, a * b * 2);
	rotateMatrix->SetElement(0, 2, a * c * 2);
	rotateMatrix->SetElement(0, 3, 0);
	// 第二行
	rotateMatrix->SetElement(1, 0, a * b * 2);
	rotateMatrix->SetElement(1, 1, square(b) + (1 - square(b)) * -1);
	rotateMatrix->SetElement(1, 2, b * c * 2);
	rotateMatrix->SetElement(1, 3, 0);
	// 第三行
	rotateMatrix->SetElement(2, 0, a * c * 2);
	rotateMatrix->SetElement(2, 1, b * c * 2);
	rotateMatrix->SetElement(2, 2, square(c) + (1 - square(c)) * -1);
	rotateMatrix->SetElement(2, 3, 0);
	// 第四行
	rotateMatrix->SetElement(3, 0, 0);
	rotateMatrix->SetElement(3, 1, 0);
	rotateMatrix->SetElement(3, 2, 0);
	rotateMatrix->SetElement(3, 3, 1);
}

void Registration::RotateByAnyAngle(double* axis, double theta, vtkSmartPointer<vtkMatrix4x4>& rotateMatrix)
{
	// 设定旋转矩阵
	double a = axis[0];
	double b = axis[1];
	double c = axis[2];
	// 第一行
	rotateMatrix->SetElement(0, 0, square(a) + (1 - square(a)) * cos(theta));
	rotateMatrix->SetElement(0, 1, a * b * (1 - cos(theta)) + c * sin(theta));
	rotateMatrix->SetElement(0, 2, a * c * (1 - cos(theta)) - b * sin(theta));
	rotateMatrix->SetElement(0, 3, 0);
	// 第二行
	rotateMatrix->SetElement(1, 0, a * b * (1 - cos(theta)) - c * sin(theta));
	rotateMatrix->SetElement(1, 1, square(b) + (1 - square(b)) * cos(theta));
	rotateMatrix->SetElement(1, 2, b * c * (1 - cos(theta)) + a * sin(theta));
	rotateMatrix->SetElement(1, 3, 0);
	// 第三行
	rotateMatrix->SetElement(2, 0, a * c * (1 - cos(theta)) + b * sin(theta));
	rotateMatrix->SetElement(2, 1, b * c * (1 - cos(theta)) - a * sin(theta));
	rotateMatrix->SetElement(2, 2, square(c) + (1 - square(c)) * cos(theta));
	rotateMatrix->SetElement(2, 3, 0);
	// 第四行
	rotateMatrix->SetElement(3, 0, 0);
	rotateMatrix->SetElement(3, 1, 0);
	rotateMatrix->SetElement(3, 2, 0);
	rotateMatrix->SetElement(3, 3, 1);
}

void Registration::TranslateMatrix(double* start, double* end, vtkSmartPointer<vtkMatrix4x4>& translateMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
				translateMatrix->SetElement(i, j, 1);
			else if (j == 3)
				translateMatrix->SetElement(i, j, start[i] - end[i]);
			else
				translateMatrix->SetElement(i, j, 0);
		}
	}
}


void Registration::TransformPoints(vtkSmartPointer<vtkMatrix4x4> matrix, vtkSmartPointer<vtkPoints> primitivePoints, vtkSmartPointer<vtkPoints>& transformedPoints)
{
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(matrix);
	transform->TransformPoints(primitivePoints, transformedPoints);
}

void Registration::TransformPoints(vtkSmartPointer<vtkMatrix4x4> matrix, double* primitivePoint, double* transformedPoint)
{
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(matrix);
	transform->TransformPoint(primitivePoint, transformedPoint);
}

void Registration::TransformPolyData(vtkSmartPointer<vtkMatrix4x4> matrix, vtkSmartPointer<vtkPolyData>& target)
{
	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(matrix);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(source);
	transformFilter->SetTransform(transform);
	transformFilter->Update();

	target->ShallowCopy(transformFilter->GetOutput());
}

void Registration::TransformPolyData(vtkSmartPointer<vtkMatrix4x4> matrix, vtkSmartPointer<vtkPolyData> source, vtkSmartPointer<vtkPolyData>& target)
{
	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(matrix);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(source);
	transformFilter->SetTransform(transform);
	transformFilter->Update();

	target->ShallowCopy(transformFilter->GetOutput());
}

void Registration::IterativeClosestPointsTransform(vtkSmartPointer<vtkPolyData> target, vtkSmartPointer<vtkMatrix4x4>& matrix, int maxIterations)
{
	// Setup ICP transform
	vtkSmartPointer<vtkIterativeClosestPointTransform> icp =
		vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	icp->GetLandmarkTransform()->SetModeToRigidBody();
	icp->CheckMeanDistanceOn();
	icp->SetCheckMeanDistance(1);
	icp->SetMaximumMeanDistance(MAXMUMMEANDISTENCE);
	icp->SetMaximumNumberOfLandmarks(5000);

	// Begin iterate
	for (int i = 0; i < maxIterations; i++)
	{
		icp->SetSource(source);
		icp->SetTarget(target);
		icp->SetMaximumNumberOfIterations(2);
		icp->Modified();
		icp->Update();

		vtkMatrix4x4::Multiply4x4(icp->GetMatrix(), matrix, matrix);

		vtkSmartPointer<vtkPolyData> modified = vtkSmartPointer<vtkPolyData>::New();
		TransformPolyData(matrix, modified);
		source = modified;

		if (icp->GetMeanDistance() <= MAXMUMMEANDISTENCE)
		{
			break;
		}
	}

	// Get the resulting transformation matrix (this matrix takes the source points to the target points)
	vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
	//std::cout << "The resulting matrix is: " << *m << std::endl;

	// Transform the source points by the ICP solution
	vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();

	icpTransformFilter->SetInputData(source);
	icpTransformFilter->SetTransform(icp);
	icpTransformFilter->Update();

	/*
	// If you need to take the target points to the source points, the matrix is:
	icp->Inverse();
	vtkSmartPointer<vtkMatrix4x4> minv = icp->GetMatrix();
	std::cout << "The resulting inverse matrix is: " << *minv << std::cout;
	*/
	matrix = m;
}

vtkSmartPointer<vtkPolyData> Registration::ConvertToVertex()
{
	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =
		vtkSmartPointer<vtkVertexGlyphFilter>::New();
	vertexFilter->SetInputData(source);
	vertexFilter->Update();

	return vertexFilter->GetOutput();
}