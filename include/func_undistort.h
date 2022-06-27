// opencv
#include <opencv2/core/version.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

// vpi
#include <vpi/OpenCVInterop.hpp>
#include <vpi/Image.h>
#include <vpi/LensDistortionModels.h>
#include <vpi/Status.h>
#include <vpi/Stream.h>
#include <vpi/algo/ConvertImageFormat.h>
#include <vpi/algo/Remap.h>
#include <vpi/algo/PerspectiveWarp.h>

// rapid json
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

// other libs
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <time.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>

// my_func
#include "my_json.h"

using namespace std;
using namespace rapidjson;
namespace fs = std::filesystem;

#define CHECK_STATUS(STMT)                                    \
    do                                                        \
    {                                                         \
        VPIStatus status = (STMT);                            \
        if (status != VPI_SUCCESS)                            \
        {                                                     \
            char buffer[VPI_MAX_STATUS_MESSAGE_LENGTH];       \
            vpiGetLastStatusMessage(buffer, sizeof(buffer));  \
            std::ostringstream ss;                            \
            ss << vpiStatusGetName(status) << ": " << buffer; \
            throw std::runtime_error(ss.str());               \
        }                                                     \
    } while (0);

class Func_Undistort
{
public:
    Func_Undistort(string json_path, string video_path, string vpi_video_path);
    void Make_remap();
    void Make_Video(int &video_width, int &video_height);

private:
    string _video_path;
    string _vpi_video_path = "output.mp4";
    float _vpi_k1;
    float _vpi_k2;
    float _x_scale;
    float _y_scale;
    float _x_rotate;
    float _y_rotate;
    float _zx_perspect;
    float _zy_perspect;
    float _x_pad;
    float _y_pad;
    int _x_focus;
    int _y_focus;

    VPIStream _stream = NULL;
    VPIPayload _remap = NULL;
    VPIImage _tmpIn = NULL, _tmpOut = NULL;
    VPIImage _vimg = nullptr;
};