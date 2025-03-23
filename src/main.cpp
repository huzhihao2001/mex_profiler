#define _USE_MATH_DEFINES

#include <mat.h>
#include <iostream>
#include <chrono>
#include "mex.h"

int main()
{
	int n = 5;//调用5次

	//读取数据
	std::cout << "读取数据：";
	auto start = std::chrono::steady_clock::now();
	const char* matfile = "../data/test.mat";
	MATFile* pmatfile = matOpen(matfile, "r");
	if (!pmatfile)
	{
		std::cerr << "Error opening file " << matfile << std::endl;
		exit(EXIT_FAILURE);
	}

	bool flag = false;
	mxArray* varargin = matGetVariable(pmatfile, "varargin");
	if (!varargin) flag = true;

	int nrhs = mxGetScalar(matGetVariable(pmatfile, "nrhs"));
	if (!nrhs || mxGetNumberOfElements(varargin) != nrhs) flag = true;
	int nlhs = mxGetScalar(matGetVariable(pmatfile, "nlhs"));
	if (!nlhs) flag = true;

	if (flag) {
		std::cerr << "数据有问题 " << std::endl;
		exit(EXIT_FAILURE);
	}
	mxArray** plhs = new mxArray * [nlhs];
	mxArray** prhs = new mxArray * [nrhs];
	for (int i = 0; i < nrhs; i++) prhs[i] = mxGetCell(varargin, i);
	auto end = std::chrono::steady_clock::now();
	std::cout << "耗时" << std::chrono::duration<double, std::milli>(end - start).count() << "毫秒" << std::endl;

	//测试时间
	std::cout << "测试时间：" << std::endl;
	for (int i = 0; i < n; i++) {
		start = std::chrono::steady_clock::now();
		mexFunction(nlhs, plhs, nrhs, const_cast<const mxArray**>(prhs));
		end = std::chrono::steady_clock::now();
		std::cout << "运行次数:" << i + 1 << "/" << n << ",耗时" << std::chrono::duration<double, std::milli>(end - start).count() << "毫秒" << std::endl;
	}

	//释放空间
	mxDestroyArray(varargin);
	for (int i = 0; i < nlhs; i++) mxDestroyArray(plhs[i]);
	delete[] plhs;
}