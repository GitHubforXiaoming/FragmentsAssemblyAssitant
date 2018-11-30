#pragma once

#include <vtkSmartPointer.h>
#include <vtkPCAStatistics.h>

#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkTransform.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkMath.h>
#include <vtkDoubleArray.h>
#include <vtkTable.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkMatrix3x3.h>
#include <vtkMatrix4x4.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>

#include <list>
#include <map>
#include <direct.h>

#define square(a) (a) * (a)
#define MAXMUMMEANDISTENCE 0.002 
#define MAXITERATIONS 80

using namespace std;

class Registration
{
private:
	int fragmentsSize, fracturesSize;
	int** fractures;
	vtkSmartPointer<vtkPoints> points;
	vtkSmartPointer<vtkPolyData> source;

	vtkSmartPointer<vtkMatrix4x4> rotateX; // 绕X轴旋转矩阵
	vtkSmartPointer<vtkMatrix4x4> rotateY; // 绕Y轴旋转矩阵
	vtkSmartPointer<vtkMatrix4x4> rotateZ; // 绕Z轴旋转矩阵

	double Multiply(double* a, double* b);
	double Norm(double* a);
	void Normalize(double* a, double* b);

	/********************************
	* 函数名：GetRotateXXX

	* 描述：获得绕各个坐标轴旋转的矩阵

	* 作者：小明

	* 日期：2018-4-9 21:34:56

	********************************/
	vtkSmartPointer<vtkMatrix4x4> GetRotateX(double theta);
	vtkSmartPointer<vtkMatrix4x4> GetRotateY(double theta);
	vtkSmartPointer<vtkMatrix4x4> GetRotateZ(double theta);


public:
	Registration();
	Registration(int fragmentsSize, int fracturesSize);
	Registration(vtkSmartPointer<vtkPolyData> source);
	~Registration();


	vector<string> SplitPair(string str, string pattern);
	vector<string> SplitString(const string str, const string pattern);
	vector<int> GetNextFracture(int fragmentId, vector<pair<string, string>> pairs);
	/********************************
	* 函数名：MatchStrategy

	* 描述：输入：一个完整模型的若干碎片，以及一个碎片上的若干断面；所有断面间的匹配结果

	输出：匹配的序列


	* 作者：小明

	* 日期：2018-5-16 15:56:21

	********************************/
	void MatchStrategy(vector<pair<string, string>> pairs, vector<int>& sequences);

	/********************************
	* 函数名：InstanceMatrix

	* 描述：初始化旋转矩阵

	* 作者：小明

	* 日期：2018-4-10 09:32:51

	********************************/
	void InstanceMatrix(vtkSmartPointer<vtkMatrix4x4>& matrix);


	/********************************
	* 函数名：PCA

	* 描述：求断面的主方向

	* 作者：小明

	* 日期：2018-4-8 09:17:43

	********************************/
	void PCA(vtkSmartPointer<vtkDoubleArray>& eigenvectors);



	/********************************
	* 函数名：RotateByAnyAxis

	* 描述：将一个向量绕任意轴旋转
	旋转向量vec使其与axis平行
	因为是绕轴旋转，所以旋转轴选择为两个向量的合成，再将某一向量绕该合成的向量旋转180°即可
	https://blog.csdn.net/swety_gxy/article/details/73087848

	* 作者：小明

	* 日期：2018-4-8 12:27:16

	********************************/
	void RotateByAnyAxis(double* axis, double* vec, vtkSmartPointer<vtkMatrix4x4>& rotateMatrix);

	/********************************
	* 函数名：RotateByAnyAngle

	* 描述：旋转向量vec使其与axis平行

	* 作者：小明

	* 日期：2018-4-8 12:27:16

	********************************/
	void RotateByAnyAngle(double* axis, double theta, vtkSmartPointer<vtkMatrix4x4>& rotateMatrix);


	/********************************
	* 函数名：TranslateMatrix

	* 描述：平移一定距离

	* 作者：小明

	* 日期：2018-4-9 16:30:27

	********************************/
	void TranslateMatrix(double* start, double* end, vtkSmartPointer<vtkMatrix4x4>& translateMatrix);


	/********************************
	* 函数名：TransformPoints

	* 描述：将变换矩阵应用在点上

	* 作者：小明

	* 日期：2018-4-10 10:05:22

	********************************/
	void TransformPoints(vtkSmartPointer<vtkMatrix4x4> matrix, vtkSmartPointer<vtkPoints> primitivePoints, vtkSmartPointer<vtkPoints>& transformedPoints);
	void TransformPoints(vtkSmartPointer<vtkMatrix4x4> matrix, double* primitivePoint, double* transformedPoint);


	/********************************
	* 函数名：TransformPolyData

	* 描述：在PolyData上应用变换矩阵，将source利用旋转矩阵旋转到target

	* 作者：小明

	* 日期：2018-4-10 14:11:57

	********************************/
	void TransformPolyData(vtkSmartPointer<vtkMatrix4x4> matrix, vtkSmartPointer<vtkPolyData>& target);
	void TransformPolyData(vtkSmartPointer<vtkMatrix4x4> matrix, vtkSmartPointer<vtkPolyData> source, vtkSmartPointer<vtkPolyData>& target);


	/********************************
	* 函数名：IterativeClosestPointsTransform

	* 描述：ICP，其中source为需要移动的PolyData，target为固定不动PolyData

	* 作者：小明

	* 日期：2018-4-10 11:58:36

	********************************/
	void IterativeClosestPointsTransform(vtkSmartPointer<vtkPolyData> target, vtkSmartPointer<vtkMatrix4x4>& matrix, int maxIterations);

	/********************************
	* 函数名：ConvertToVertex

	* 描述：以点集形式显示模型

	* 作者：小明

	* 日期：2018-4-9 10:17:52

	********************************/
	vtkSmartPointer<vtkPolyData> ConvertToVertex();



	/********************************
	* 函数名：GetRotateAxis

	* 描述：获得两个向量的向量积方向，作为旋转轴

	* 作者：小明

	* 日期：2018-7-13 17:08:03

	********************************/
	void GetRotateAxis(double* axis1, double* axis2, double* axis);
};
