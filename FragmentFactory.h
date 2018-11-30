#pragma once

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include "Registration.h"

using namespace std;

class FragmentFactory
{
private:
	Registration registration;

	vector<vtkSmartPointer<vtkPolyData>> fragments;
	vector<vector<vtkSmartPointer<vtkPolyData>>> fracturess;
public:
	FragmentFactory();
	~FragmentFactory();

	void SetData(vector<vtkSmartPointer<vtkPolyData>> fragments, vector<vector<vtkSmartPointer<vtkPolyData>>> fracturess);
	
};
