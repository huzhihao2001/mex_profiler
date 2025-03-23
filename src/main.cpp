#define _USE_MATH_DEFINES

#include <mat.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include "mex.h"

int main() {
    const int n = 5;  // 调用5次

    // 使用智能指针管理MAT文件资源（自定义删除器）
    auto mat_closer = [](MATFile* p) { if (p) matClose(p); };
    using MatFilePtr = std::unique_ptr<MATFile, decltype(mat_closer)>;

    // 读取数据
    std::cout << "读取数据：";
    auto start = std::chrono::steady_clock::now();

    const char* matfile = "../data/test.mat";
    MatFilePtr pmatfile(matOpen(matfile, "r"), mat_closer);
    if (!pmatfile) {
        std::cerr << "Error opening file " << matfile << std::endl;
        exit(EXIT_FAILURE);
    }

    // 使用智能指针管理mxArray资源
    using MxArrayDeleter = void(*)(mxArray*);
    using MxArrayPtr = std::unique_ptr<mxArray, MxArrayDeleter>;

    MxArrayPtr varargin(matGetVariable(pmatfile.get(), "varargin"), mxDestroyArray);
    if (!varargin) {
        std::cerr << "数据有问题" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 验证数据完整性
    MxArrayPtr nrhs_arr(matGetVariable(pmatfile.get(), "nrhs"), mxDestroyArray);
    MxArrayPtr nlhs_arr(matGetVariable(pmatfile.get(), "nlhs"), mxDestroyArray);

    const int nrhs = mxGetScalar(nrhs_arr.get());
    const int nlhs = mxGetScalar(nlhs_arr.get());

    if (!nrhs || !nlhs || mxGetNumberOfElements(varargin.get()) != nrhs) {
        std::cerr << "数据有问题" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 使用vector管理指针数组（自动释放）
    std::vector<MxArrayPtr> plhs;
    plhs.reserve(nlhs);
    for (int i = 0; i < nlhs; ++i) {
        plhs.emplace_back(nullptr, mxDestroyArray);
    }

    // 注意：这里prhs只是借用varargin的数据，不拥有所有权
    std::vector<mxArray*> prhs(nrhs);
    for (int i = 0; i < nrhs; ++i) {
        prhs[i] = mxGetCell(varargin.get(), i);
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "耗时" << std::chrono::duration<double, std::milli>(end - start).count() << "毫秒" << std::endl;

    // 测试时间
    std::cout << "测试时间：" << std::endl;
    for (int i = 0; i < n; ++i) {
        start = std::chrono::steady_clock::now();

        // 转换智能指针到原始指针数组
        std::vector<mxArray*> plhs_raw;
        plhs_raw.reserve(plhs.size());
        for (auto& p : plhs) {
            plhs_raw.push_back(p.get());
        }

        mexFunction(nlhs, plhs_raw.data(), nrhs, const_cast<const mxArray**>(prhs.data()));

        // 接管mexFunction分配的内存
        for (size_t j = 0; j < plhs.size(); ++j) {
            plhs[j].reset(plhs_raw[j]);
        }

        end = std::chrono::steady_clock::now();
        std::cout << "运行次数:" << i + 1 << "/" << n << ",耗时"
            << std::chrono::duration<double, std::milli>(end - start).count()
            << "毫秒" << std::endl;
    }
}