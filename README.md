# I am making a game engine
The game made inside of this engine will feature hundreds of instances of flying boids. Therefore I have placed some severe limitations on the engine implementation in the name of performance and shorter development time. This will not sacrifice future extendability, as the engine is very modular.

### Static geometry only
Vbos, ibos and vaos must be properly and fully initialized with valid data to be constructed. They cannot be modified afterwards. 

### Batch data before use
Batch vertex data and index data before passing it to the vbo and ibo. 

### Have vertex data layout however you want (AoS or SoA)
Either have all vertex attributes already in-line inside the model data (AoS). Or have them as separate arrays (SoA).

### Write shaders however you want 
You can define multiple shaders in the same file, in whatever order you like. Use the '#shader' specifier to declare shader type. Include shader declarations into shader source files with the '#include' directive.

### Custom .obj parser
The game will not feature any skeletal animations, therefore the .obj-format will suffice. 

### Mapped Inputs
Bind actions dynamically to multiple keys at once. 
