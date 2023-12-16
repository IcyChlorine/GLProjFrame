## v0.3.0: grid and decent application

+ add a grid in viewport, serving as a virtual ground
+ basic MSAA(multi-sample anti-aliasing) to window
+ handle vsync properly
+ remove class `Transform`, which wraps glm matrices too much
+ a thorough update to `InputManager`
  + now support multiple callbacks registered to one events
  + now keep track of mouse and key status and window size
  + now support mouse middle key and mouse scroll
+ implement a CAD-like camera that orients around the object being inspected, called `InspectCamera`. 
  + It's more similar to cameras in *blender* than to cameras in *fps games*. 
  + drag to orient around certain objects in the scene, and WASD to wander.
  + initiated in a diagonal upward perspective, so as to view objects more clearly.
+ separate a common base class for applications, called `GraphicsApplication`. 
+ implement `DemoApp`(a son of `GraphicsApplication`), a decent demo app with the grid as ground and the iconic initial cube.
+ further dev work will proceed in `MyApp`.





## v0.2.0: robust mesh loading

+ upgrade to 64 bit
+ start to resolve third-party library dependencies automatically
+ more robust mesh loading
  + print functions for `assimp` basics
  + upgrade to `assimp` v5.2.3
  + now support opacity map
+ add `Material` class, for handling all material parameters and related textures
+ support dynamic shader reloading(`R` key), facilitating shading debug
+ better Phong shading
+ add fullscreen mode, toggle by `F11`
+ a big upgrade to camera logic
+ now support model transform (translate, local rotate and local scale)
+ now build-related files(mostly object files) will be written into `GLProjFrame/build` directory