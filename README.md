# GLProjFrame
## Overall View

Graphic-oriented Programs based on OpenGL are usually tedious to make/build*(even when you focus on Windows)*, because of the old-fashioned OpenGL API is C-styled and you'll have to depend on various third-party libraries to implement functions like mesh-loading. This project is designed to help solve these problems by:

* Providing a set of easy-to-use C++ wrapper classes called `glpp` for OpenGL objects, such as `class Shader` for constructing shaders  and a set of util classes such as `class Camera`.
* Providing plug-and-play scripts for bringing common third-party libraries(such as `glfw`, `glm`, and`stb_image`) into the project, so as to exempt developers the trouble of downloading and building these libraries on their own. They're now part of the makefile in the project.

**Current Version: v0.1.1**

> About version: Recently I've had a much bigger and more ambitious plan. Realizing that the project is too far from complete, the old "v1.0" version is now "v0.1.0", and the current version on MinGW is v0.1.1.

## Make

Recently I've been working on **MinGW-w64** environment. And the project use `make` to build:

> Though I previously started the project in Visual Studio, I found the GNU suite provides **cleaner interface** and a **more thorough and transparent control** over the build process.
>
> Moreover, it's easy to switch to VS by switching to CMakelists.txt from makefile, while the reverse process is usually harder. There's plan to switch to CMake in future, when the project gets big and complicated.

```bash
$ git clone git@github.com:IcyChlorine/GLProjFrame.git
$ cd GLProjFrame/GLProjFrame
# download, build and install third-party dependencies
$ make assimp_install
$ make freetype_install
# now build GLProjFrame
$ make .
```

The main project is `GLProjFrame` in the folder with the same name; Note that an auxiliary project `test` is also included, as a handy tool for developers to test any weird or interesting programming cases within the same solution. To run `test`, run the following command:

```bash
# root
$ make test
```



## C++ Wrapper Classes: glpp

The OpenGL was designed as a set of C-styled API, making some of its functions obscure and hard to use.*(and so are many of its popular util libs)* To make things better, I've written a set of C++ wrapper classes, which is called **glpp**. Some typical classes and their functions are listed as follow:

+ `class AbsObject` An abstracted class equipped with ```AbsObject* father``` to support many useful features. Also, a virtual funciton `virtual void outputDebugInfo(ostream& out)` is added.

+ `class Shader` A wrapper class for OpenGL shaders. You can directly create a shader by `Shader shader("src/shader/vertex.glsl","src/shader/frag.glsl")` and the constructor will do all the file-reading, compiling, linking jobs for you by calling OpenGL libraries. Then you can use it by `shader.use()` and pass uniform variables to it by `shader.setUniform(...)`.

+ `class Application` The ultimate abstraction of GL program, which makes the main function concise and easy to maintain. It 's recommended to derive a new class from `class Application` for your custom use. Generally, it functions like this:

```C++
...
#include "Application.h"
...

int main() {
  	try {
  		Application theApp;
  		theApp.run();
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

 ## Project Structure

With dependent libraries, `glpp` classes and util sources added to the project, the whole framework form a structure like this:

```
+----------------------------------+
|        Application->run()        |	Layer IV: Application
+----------------------------------+
|   glpp Classes: Camera, Shader   |
|     Transform, Shader, ...       |	Layer III:glpp wrapper classes as abstraction
+--------------------+-------------+
| External Lib: glfw | util: ...   |
|   glad glad stb ...|  LogWriter  |	Layer II: Basic Dependencies & Extensions
+--------------------+-------------+	
|  Basic GL Lib:     | Basic C Lib |
|   gl.h, glut.h     |   STL, ...  |	Layer I:  Foundation
+--------------------+-------------+
```
