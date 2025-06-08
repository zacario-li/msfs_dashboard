# MSFS Dashboard

这是一个微软模拟飞行2020/2024的仪表盘程序。  
我儿子特别喜欢模拟飞行，但很多操作靠手柄不是那么直接和方便，所以做了几个他常用的控制按钮，并根据他的要求设定了一些习惯。

## 截图

### 仪表盘

![Dashboard](dashboard.png)

### 游戏内截图

![Screenshot MSFS](screenshot_msfs.png)

## 构建

[构建说明]  
install Qt6.9 or anyother version, and modify the path in CMakeLists.txt  
intall SimConnect SDK  
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 使用
[使用说明]  
编译后，在build文件夹中双击MSFSDashboard.exe即可（对了，你必须先启动MSFS2020或者2024）

## 许可证

[许可证] 
AGPLv3
