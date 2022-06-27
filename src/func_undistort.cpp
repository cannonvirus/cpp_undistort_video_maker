#include "func_undistort.h"

using namespace std;
using namespace rapidjson;
namespace fs = std::filesystem;

Func_Undistort::Func_Undistort(string json_path, string video_path, string vpi_video_path)
{

    map<string, variant<string, int, float>> param_dict;
    json_reader(json_path, param_dict, false);

    _vpi_k1 = get<float>(param_dict["vpi_k1"]);
    _vpi_k2 = get<float>(param_dict["vpi_k2"]);
    _x_scale = get<float>(param_dict["x_scale"]);
    _y_scale = get<float>(param_dict["y_scale"]);
    _x_rotate = get<float>(param_dict["x_rotate"]);
    _y_rotate = get<float>(param_dict["y_rotate"]);
    _zx_perspect = get<float>(param_dict["zx_perspect"]);
    _zy_perspect = get<float>(param_dict["zy_perspect"]);
    _x_pad = get<float>(param_dict["x_pad"]);
    _y_pad = get<float>(param_dict["y_pad"]);
    _x_focus = get<int>(param_dict["x_focus"]);
    _y_focus = get<int>(param_dict["y_focus"]);
    _video_path = video_path;
    _vpi_video_path = vpi_video_path;
}

void Func_Undistort::Make_remap()
{
    cv::Size imgSize;
    cv::VideoCapture capture(_video_path);
    cv::Mat img;

    if (!capture.isOpened())
    {
        printf("MP4 file can not open.\n");
    }
    imgSize.width = capture.get(3);
    imgSize.height = capture.get(4);

    capture >> img;

    using Mat3 = cv::Matx<double, 3, 3>;
    Mat3 camMatrix = Mat3::eye();
    camMatrix(0, 0) = 10.0;
    camMatrix(1, 1) = 10.0;
    if (_x_focus == 0)
        camMatrix(0, 2) = imgSize.width / 2.0;
    else
        camMatrix(0, 2) = _x_focus;
    if (_y_focus == 0)
        camMatrix(1, 2) = imgSize.height / 2.0;
    else
        camMatrix(1, 2) = _y_focus;

    VPICameraIntrinsic K;
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            K[i][j] = camMatrix(i, j);
        }
    }

    VPICameraExtrinsic X = {};
    X[0][0] = _x_scale;
    X[1][1] = _y_scale;

    // ? rotate
    X[0][1] = _x_rotate;
    X[1][0] = _y_rotate;

    // ? pad
    X[0][3] = _x_pad;
    X[1][3] = _y_pad;

    // ? z축
    X[2][0] = _zx_perspect;
    X[2][1] = _zy_perspect;

    X[2][2] = 1;

    VPIWarpMap map = {};
    map.grid.numHorizRegions = 1;
    map.grid.numVertRegions = 1;
    map.grid.regionWidth[0] = imgSize.width;
    map.grid.regionHeight[0] = imgSize.height;
    map.grid.horizInterval[0] = 1;
    map.grid.vertInterval[0] = 1;
    CHECK_STATUS(vpiWarpMapAllocData(&map));

    VPIPolynomialLensDistortionModel distModel = {};
    // coeffs[0] = coeffs_value;
    distModel.k1 = _vpi_k1;
    distModel.k2 = _vpi_k2;
    distModel.k3 = 0;
    distModel.k4 = 0;

    vpiWarpMapGenerateFromPolynomialLensDistortionModel(K, X, K, &distModel, &map);
    CHECK_STATUS(vpiCreateRemap(VPI_BACKEND_CUDA, &map, &_remap));
    vpiWarpMapFreeData(&map);
    CHECK_STATUS(vpiStreamCreate(VPI_BACKEND_CUDA, &_stream));
    CHECK_STATUS(vpiImageCreate(imgSize.width, imgSize.height, VPI_IMAGE_FORMAT_NV12_ER, 0, &_tmpIn));
    CHECK_STATUS(vpiImageCreate(imgSize.width, imgSize.height, VPI_IMAGE_FORMAT_NV12_ER, 0, &_tmpOut));

    if (_vimg == nullptr)
    {
        CHECK_STATUS(vpiImageCreateOpenCVMatWrapper(img, 0, &_vimg));
    }
    else
    {
        CHECK_STATUS(vpiImageSetWrappedOpenCVMat(_vimg, img));
    }

    CHECK_STATUS(vpiSubmitConvertImageFormat(_stream, VPI_BACKEND_CUDA, _vimg, _tmpIn, NULL));
    CHECK_STATUS(vpiSubmitRemap(_stream, VPI_BACKEND_CUDA, _remap, _tmpIn, _tmpOut, VPI_INTERP_CATMULL_ROM,
                                VPI_BORDER_ZERO, 0));
    CHECK_STATUS(vpiSubmitConvertImageFormat(_stream, VPI_BACKEND_CUDA, _tmpOut, _vimg, NULL));
    CHECK_STATUS(vpiStreamSync(_stream));

    capture.release();

    cout << " ***** Remap Function make finish ***** " << endl;
}

void Func_Undistort::Make_Video(int &video_width, int &video_height)
{

    cv::VideoCapture capture(_video_path);
    cv::Mat img;

    cv::VideoWriter wr_video(_vpi_video_path,
                             cv::VideoWriter::fourcc('X', '2', '6', '4'),
                             30, cv::Size(capture.get(3), capture.get(4)));

    video_width = capture.get(3);
    video_height = capture.get(4);

    clock_t start, end;
    double result;

    start = clock();
    while (true)
    {
        capture >> img;
        if (img.empty())
        {
            capture.release();
            wr_video.release();
            break;
        }

        if (_vimg == nullptr)
        {
            CHECK_STATUS(vpiImageCreateOpenCVMatWrapper(img, 0, &_vimg));
        }
        else
        {
            CHECK_STATUS(vpiImageSetWrappedOpenCVMat(_vimg, img));
        }

        CHECK_STATUS(vpiSubmitConvertImageFormat(_stream, VPI_BACKEND_CUDA, _vimg, _tmpIn, NULL));
        CHECK_STATUS(vpiSubmitRemap(_stream, VPI_BACKEND_CUDA, _remap, _tmpIn, _tmpOut, VPI_INTERP_CATMULL_ROM,
                                    VPI_BORDER_ZERO, 0));
        CHECK_STATUS(vpiSubmitConvertImageFormat(_stream, VPI_BACKEND_CUDA, _tmpOut, _vimg, NULL));
        CHECK_STATUS(vpiStreamSync(_stream));
        wr_video.write(img);
    }
    vpiStreamDestroy(_stream);
    vpiPayloadDestroy(_remap);
    vpiImageDestroy(_tmpIn);
    vpiImageDestroy(_tmpOut);
    vpiImageDestroy(_vimg);
    end = clock();
    result = (double)(end - start);
    cout << "result : " << ((result) / CLOCKS_PER_SEC) << " seconds" << endl;
    cout << " ***** Finish Making Video *****" << endl;
}
