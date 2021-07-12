# GLProjFrame
## Overall View

Graphic-oriented Programs based on OpenGL are usually tedious to make/build*(even when you focus only on Visual Studio in Windows)*, because of the old-fashioned OpenGL API which is C-styled and various library and header files dependencies. This project is designed to help solve these problems by:

* Providing a set of C++ wrapper classes for OpenGL objects such as `class Shader` and a set of util classes such as `class Camera`
* Taking popular libraries within the project,such as `glfw`,`glm`,and`stb`,so as to exempt developers the trouble of downloading,building these libraries on their own. You can find them in `/GLProjFrame/[src,include,lib]`
* Using CMake make scripts to achieve better portability.  Click `ClickMetoCMake.bat` to make project files.  Visual Studio 2010+ is recommended.

## Latest Version

**Current version**: `v1.0`

**What's new**:

+ the famous mesh loading library `assimp` now incorporated! run `class MeshDemo` in `main.cpp` to check.
+ major revisions have been made to glpp, its now less obscure and more handy. Classes structure is now simpler and more de-coupled.

## Make

Now I've only configured the project for VS in Windows. I intended to add support for more platforms/IDEs, but considering the complexity of building an OpenGL project, I still recommend Visual Studio as your first choice.

Use **CMake** to make project files. All library and header files are already included in this repository. Visual Studio 2010+ is recommended.

The main project is `GLProjFrame` in the folder with the same name; Note that an auxiliary project `test` is also included, as a handy tool for developers to test any weird or interesting programming cases within the same solution.

## C++ Wrapper Classes: glpp

The OpenGL was designed as a set of C-styled API, making some of its functions obscure and hard to use.*(and so are many of its popular util libs)* To make things better, I've written a set of C++ wrapper classes, which is called glpp. Some typical classes and their functions are listed as follow:

+ `class AbsObject` An abstracted class equipped with ```AbsObject* father``` to support many useful features. Also, a virtual funciton`virtual void outputDebugInfo(ostream& out)` is added.

+ `class Shader` A wrapper class for OpenGL shaders. You can directly create a shader by `Shader shader("src\\shader\\vertex.glsl","src\\shader\\frag.glsl")` and the constructor will do all the file-reading, compiling, linking jobs for you by calling OpenGL libraries. Then you can use it by `shader.use()` and pass uniform variables to it by `shader.setUniform(...)`.

+ `class GraphicApplicationBase` The ultimate abstraction of GL program, which makes the main function concise and easy to maintain. It 's recommended to derive a new class from `class GraphicApplicationBase` for your custom use. Generally, it functions like this:

```C++
...
#include "glpp\glpp.h"
...

int main() {
  	try {
  		MyApplication *theApp;
  		theApp->run();
  	}
  	catch (init_exception& e) {
  		HandleException(e, "init_exception");
  	}
  	catch (exception& e) {
  		HandleException(e);
  	}
  	return 0;
}
```

Be aware that you need to override `virtual void GraphicApplicationBase::run()` function to customize the behaviour of your own application.

 ## Project Structure

With dependent libraries, glpp classes and util sources added to the project, the whole framework form a structure like this:

```
+----------------------------------+
|        Application->run()        |	Layer IV: Application
+----------------------------------+
|   glpp Classes: Camera, Shader   |
|     Transform, Shader, ...       |	Layer III:glpp wrapper classes as abstraction
+--------------------+-------------+
| External Lib:      | util: ...   |
|   assimp std_image |   Logging   |	Layer II: Basic Dependencies&Extensions
|   glad glfw glm    |             |
+--------------------+-------------+	
|  Basic GL Lib:     | Basic C Lib |
|   gl.h, glut.h     |   STL, ...  |	Layer I:  Foundation
+--------------------+-------------+
```



### Readme Legacy from alpha version

*之前在开发alpha版本时留下的一些readme和改动信息*

**以下所有版本现均改为alpha(~v1.8)，现版本为beta.0.**

一个openGL程序的框架
包含一些基本类:
class Shader;
class Texture;
class Console;
	保存键鼠信息
class GameTransform;
	让你在3D世界中用键鼠控制视角，方便debug
	
自带了一些常用的库和头文件
	glfw3,glad,glm
并且在项目中配置好了。

在VS2015中创建项目。

2020-10-2 note
	粗略完成了v2(目前我称之为v1.8).还有一些可以改进的小部分：
	class Transform.enable(bool)的问题
	在场景下加入地表坐标格和坐标轴
	在窗口加入HUD
	InputManager加入registerListener(允许多个监听器)和clearListener
		并将GameCamera中的glfw..也加入
	一些语法的修正
	一些文件尚未删除，文件夹结构待优化的空间
	升级到64位
	最后，改造成可以用cmake一键生成项目的样子
	
v1.8
	加入了AbsObject-一个可以设置父类指针father的基类，便于实现层次结构
	优化了头文件引用，现在从c，stl到gl类库都由common_t1.h包含
	加入了init_exception, LogWriter等类，现在终于有一个上台面的错误处理系统了
	对类的结构进行了大量优化，比第一代有更强的“高内聚，低耦合”的特性；
		充分运用了java中的程序设计思想，用继承使类间结构更清晰
	=>现在有一个比较完善的C++-styled gl封装层了
	一些新老类名对照：
	GameTransform-> Camera & Transform
	GLFWwindow* -> Window