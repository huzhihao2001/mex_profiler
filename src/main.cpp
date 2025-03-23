#define _USE_MATH_DEFINES

#include <mat.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include "mex.h"

int main() {
    const int n = 5;  // ����5��

    // ʹ������ָ�����MAT�ļ���Դ���Զ���ɾ������
    auto mat_closer = [](MATFile* p) { if (p) matClose(p); };
    using MatFilePtr = std::unique_ptr<MATFile, decltype(mat_closer)>;

    // ��ȡ����
    std::cout << "��ȡ���ݣ�";
    auto start = std::chrono::steady_clock::now();

    const char* matfile = "../data/test.mat";
    MatFilePtr pmatfile(matOpen(matfile, "r"), mat_closer);
    if (!pmatfile) {
        std::cerr << "Error opening file " << matfile << std::endl;
        exit(EXIT_FAILURE);
    }

    // ʹ������ָ�����mxArray��Դ
    using MxArrayDeleter = void(*)(mxArray*);
    using MxArrayPtr = std::unique_ptr<mxArray, MxArrayDeleter>;

    MxArrayPtr varargin(matGetVariable(pmatfile.get(), "varargin"), mxDestroyArray);
    if (!varargin) {
        std::cerr << "����������" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ��֤����������
    MxArrayPtr nrhs_arr(matGetVariable(pmatfile.get(), "nrhs"), mxDestroyArray);
    MxArrayPtr nlhs_arr(matGetVariable(pmatfile.get(), "nlhs"), mxDestroyArray);

    const int nrhs = mxGetScalar(nrhs_arr.get());
    const int nlhs = mxGetScalar(nlhs_arr.get());

    if (!nrhs || !nlhs || mxGetNumberOfElements(varargin.get()) != nrhs) {
        std::cerr << "����������" << std::endl;
        exit(EXIT_FAILURE);
    }

    // ʹ��vector����ָ�����飨�Զ��ͷţ�
    std::vector<MxArrayPtr> plhs;
    plhs.reserve(nlhs);
    for (int i = 0; i < nlhs; ++i) {
        plhs.emplace_back(nullptr, mxDestroyArray);
    }

    // ע�⣺����prhsֻ�ǽ���varargin�����ݣ���ӵ������Ȩ
    std::vector<mxArray*> prhs(nrhs);
    for (int i = 0; i < nrhs; ++i) {
        prhs[i] = mxGetCell(varargin.get(), i);
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "��ʱ" << std::chrono::duration<double, std::milli>(end - start).count() << "����" << std::endl;

    // ����ʱ��
    std::cout << "����ʱ�䣺" << std::endl;
    for (int i = 0; i < n; ++i) {
        start = std::chrono::steady_clock::now();

        // ת������ָ�뵽ԭʼָ������
        std::vector<mxArray*> plhs_raw;
        plhs_raw.reserve(plhs.size());
        for (auto& p : plhs) {
            plhs_raw.push_back(p.get());
        }

        mexFunction(nlhs, plhs_raw.data(), nrhs, const_cast<const mxArray**>(prhs.data()));

        // �ӹ�mexFunction������ڴ�
        for (size_t j = 0; j < plhs.size(); ++j) {
            plhs[j].reset(plhs_raw[j]);
        }

        end = std::chrono::steady_clock::now();
        std::cout << "���д���:" << i + 1 << "/" << n << ",��ʱ"
            << std::chrono::duration<double, std::milli>(end - start).count()
            << "����" << std::endl;
    }
}