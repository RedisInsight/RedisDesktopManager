### 在VirtualBox 下出现黑屏
删除RDM安装目录下面的 opengl32sw.dll 然后下载修复版的[opengl32sw.dll](https://github.com/uglide/RedisDesktopManager/blob/5f99aa22aa84dc55b5c9858de28d68facd1c58b7/build/windows/installer/resources/rdp_fix/opengl32.dll) 到相同目录下面。

### 使用Windows远程桌面出现黑屏
你需要在 “远程桌面连接” 里面的 “显示选项/显示/颜色” 调整颜色深度为32位。 其他远程桌面软件出现黑屏, 在相应设置里面调整颜色深度至32位。

### 在Linux 下出现黑屏

RDM 从0.8.0+ 开始, 要求显卡驱动支持 OpenGL。 在命令行终端 运行`redis-desktop-manager`, 搜索以下错误: `libGL error: failed to load driver: XXX`

也可以通过运行其他 OpenGL程序来测试 OpenGL支持, 例如: 运行 `glxgears`测试程序。


**解决方案**

重新安装或更新显卡驱动

### 程序启动出现 0xc000007b错误。

下载运行 [`vcredist_x86.exe`](https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe), 点击`修复`选项。

### Ubuntu (RDM 0.8.0+) 下显示不正常

这个问题是因为 QT Quick 组件导致。

### 无法在 Windows下正常启动 (RDM 0.8.6+)

RDM 需要显卡支持驱动 OpenGL 2.1及以上。请更新显卡驱动至最新版本。

如果是在虚拟机运行的Windows，请启用"3D加速" 或者 "GPU 加速"。
