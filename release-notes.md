**v0.2.0: robust mesh loading**

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