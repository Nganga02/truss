# This is the documentation for the implementation of the truss project
## Scope of the project
1. Implement the equations of truss 
1. Implement the animation for the behavior of the truss

The document captures the flow of how I implemented the project from start to finish

Equations of study:

1. ### The stress function
   $F(\lambda ) = A\sigma(\lambda)$


1. ### The stored energy function
   $W(\lambda) = \int_{1}^{\lambda} \sigma(\xi)\, d\xi$


1. ### Linear stress strain relationship
   $\sigma = E\epsilon$


### Libraries
If you are using ubuntu for development of this project
you'll require to install one library for rendering purposes that is the simple directmedia layer.


__Installation__

When you want to run the project

```sudo apt install libsdl2-2.0-0```

When developing/compiling programs with SDL

`sudo apt install libsdl2-dev`

When you want to upload images into the program 

```sudo apt install libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev```


### Pre-requisite header and source files
The following are the header files required to accomplish the truss project
```
1. nelder-mead.[ch]
2. linked-list.[ch]
3. fetch_line.[ch]
4. xmalloc.[ch]
```

### Files maneuvering
The files `truss-io.[ch]` handle the reading and writing of truss description files which has the extension `.tdf`
