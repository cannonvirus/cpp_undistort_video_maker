<h1 align="center">Welcome to cpp_undistort_video 👋</h1>
<p>
  <img alt="Version" src="https://img.shields.io/badge/version-1.0-blue.svg?cacheSeconds=2592000" />
  <a href="https://docs.nvidia.com/vpi/index.html" target="_blank">
    <img alt="Documentation" src="https://img.shields.io/badge/documentation-yes-brightgreen.svg" />
  </a>
</p>

> this project is sample code for making undistorted video to use vpi nvidia algorithm

## Install

```sh
intflow/edgefarm:room_v1.4_dp6_ubuntu18_3090

# rapidjson
# vpi1
# c++17
# opencv

```

## video_config.json

```sh
{
    "root_path": "/data/EdgeFarm_pig/Tracking/autolabel_rootpath/",
    "video_path": "/data/EdgeFarm_pig/Tracking/reference_video/room2_act.mp4",
    "interval": 8
}
```

## vpi_config.json

```sh
{
    "vpi_k1": -0.000016,
    "vpi_k2": -0.0,
    "x_scale": 1.0,
    "y_scale": 1.0,
    "x_rotate": 0.0,
    "y_rotate": 0.0,
    "zx_perspect": -0.0004,
    "zy_perspect": 0.0008,
    "x_pad": -13.0,
    "y_pad": 1.0,
    "x_focus": 1088,
    "y_focus": 566
}
```

## build

- config jsonfile change & build & use

```cpp
  string vpi_config_path = "../vpi_config.json";
  string video_config_path = "../video_config.json";
```

## Author

👤 **cannonvirus**

- Github: [@cannonvirus](https://github.com/cannonvirus)

## Show your support

Give a ⭐️ if this project helped you!

***
_This README was generated with ❤️ by [readme-md-generator](https://github.com/kefranabg/readme-md-generator)_