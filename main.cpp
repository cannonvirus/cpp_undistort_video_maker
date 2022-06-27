#include "func_undistort.h"
#include "my_json.h"

using namespace std;
namespace fs = std::filesystem;

void autolabel_foler_create(string root_path, string videoname, int interval)
{
    fs::create_directory(root_path + videoname);

    // ? full img, label, infer, track
    fs::create_directory(root_path + videoname + "/" + "full_img");
    fs::create_directory(root_path + videoname + "/" + "full_label");
    fs::create_directory(root_path + videoname + "/" + "full_infer");
    fs::create_directory(root_path + videoname + "/" + "full_track");

    // ? interval img, label, infer, track
    fs::create_directory(root_path + videoname + "/" + "interval_" + to_string(interval) + "_img");
    fs::create_directory(root_path + videoname + "/" + "interval_" + to_string(interval) + "_label");
    fs::create_directory(root_path + videoname + "/" + "interval_" + to_string(interval) + "_infer");
    fs::create_directory(root_path + videoname + "/" + "interval_" + to_string(interval) + "_track");

    // ? option
    fs::create_directory(root_path + videoname + "/" + "option");

    // ? origin
    fs::create_directory(root_path + videoname + "/" + "originfile");
}

void zfill(unsigned long long number, int length, string &str_number)
{
    ostringstream str;
    str << setw(length) << setfill('0') << number;
    str_number = str.str();
}

int main()
{

    string vpi_config_path = "../vpi_config.json";
    string video_config_path = "../video_config.json";

    // ? folder create ------------------------------------------
    map<string, variant<string, int, float>> param_dict;
    json_reader(video_config_path, param_dict, true);

    string root_path = get<string>(param_dict["root_path"]);
    string video_path = get<string>(param_dict["video_path"]);
    int interval = get<int>(param_dict["interval"]);

    fs::path tmp = video_path;
    string videoname = tmp.filename().c_str();
    videoname = videoname.substr(0, videoname.rfind("."));

    autolabel_foler_create(root_path, videoname, interval);

    string output_vpifile = root_path + videoname + "/originfile/vpi.mp4";

    // ? undistort video create -------------------------

    Func_Undistort func_undistort(vpi_config_path, video_path, output_vpifile);
    int video_width = 0;
    int video_height = 0;

    func_undistort.Make_remap();
    func_undistort.Make_Video(video_width, video_height);

    // ? video -> img
    cv::VideoCapture cap(output_vpifile);
    int framecount = 0;
    string framename;
    string full_img_path = root_path + videoname + "/full_img/";
    string interval_img_path = root_path + videoname + "/interval_" + to_string(interval) + "_img/";

    if (!cap.isOpened())
    {
        cout << "Unable to open file!" << endl;
        return 0;
    }

    while (1)
    {
        cv::Mat img;
        cap >> img;
        if (img.empty())
        {
            cout << " ***** Frame End ***** " << endl;
            break;
        }

        zfill(framecount, 5, framename);
        string imgname__ = full_img_path + framename + ".jpg";

        cv::imwrite(imgname__, img);

        if (framecount % interval == 0)
        {
            string intervalname__ = interval_img_path + framename + ".jpg";
            cv::imwrite(intervalname__, img);
            cout << " ** interval image save " << framename << ".jpg ** " << endl;
        }

        framecount++;
    }

    // ? file copy to rootpath
    string copy_originvideo_path = root_path + videoname + "/originfile/origin.mp4";
    fs::copy(video_path, copy_originvideo_path, fs::copy_options::overwrite_existing);

    string copy_vpi_config_path = root_path + videoname + "/option/vpi_config.json";
    fs::copy(vpi_config_path, copy_vpi_config_path, fs::copy_options::overwrite_existing);

    string copy_video_config_path = root_path + videoname + "/option/video_config.json";
    param_dict["x_display"] = video_width;
    param_dict["y_display"] = video_height;
    json_writer(copy_video_config_path, param_dict);

    string permission_cmd = "chmod -R 777 " + root_path + videoname;
    system(permission_cmd.c_str());
    cout << " ***** FINISH PROGRAM ***** " << endl;
}
