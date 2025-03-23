#define _USE_MATH_DEFINES

#include <mat.h>
#include <iostream>
#include <chrono>
#include "mex.h"

int main()
{
	int n = 5;//����5��

	//��ȡ����
	std::cout << "��ȡ���ݣ�";
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
		std::cerr << "���������� " << std::endl;
		exit(EXIT_FAILURE);
	}
	mxArray** plhs = new mxArray * [nlhs];
	mxArray** prhs = new mxArray * [nrhs];
	for (int i = 0; i < nrhs; i++) prhs[i] = mxGetCell(varargin, i);
	auto end = std::chrono::steady_clock::now();
	std::cout << "��ʱ" << std::chrono::duration<double, std::milli>(end - start).count() << "����" << std::endl;

	//����ʱ��
	std::cout << "����ʱ�䣺" << std::endl;
	for (int i = 0; i < n; i++) {
		start = std::chrono::steady_clock::now();
		mexFunction(nlhs, plhs, nrhs, const_cast<const mxArray**>(prhs));
		end = std::chrono::steady_clock::now();
		std::cout << "���д���:" << i + 1 << "/" << n << ",��ʱ" << std::chrono::duration<double, std::milli>(end - start).count() << "����" << std::endl;
	}

	//�ͷſռ�
	mxDestroyArray(varargin);
	for (int i = 0; i < nlhs; i++) mxDestroyArray(plhs[i]);
	delete[] plhs;
}