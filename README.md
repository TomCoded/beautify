Beautify
=====================================================

### Photon Mapper

This is a photon mapper built without substantial use of graphics libraries.

- Reads and parses complex scene files into a scene including lights, objects, shapes, materials, and fixed and dynamic transformations.
- Distributes the scene to multple processes.
- Throws thousand of photons into the scene from the lights.
- Depending on the result of a photon roullete process, photons can bounce off objects (being adjusted for the BRDF) and possibly land on other objects, or they can be absorbed. (i.e. each time a photon hits a surface, it is stored within the photon map and Russian roulette is used to determine if the photon is reflected, with the power of the reflected photon being computed based on the BRDF of the adjusted surface)
- Gathers the resulting photons' bounce locations (if not absorbed).
- Sorts the photons into a 3-dimensional kd-Tree.
- Distributes the kd-Tree to all processes.
- The first step of a ray tracing process is used: rays are shot from the camera into the scene and, if an object is hit, luminance information for that spot on the object is calculated from the photon map using the nearest N photons and assuming a 2-dimensional surface.

- The Message Passing Interface (MPI) is used for interprocess communication.
- GLUT is used to write rendered file to the screen, though a simple putPixel() from any library would work.
- ImageMagick++ is used to write rendered files to .jpg files.

### Example

*note: If your browser or the markdown viewing engine for this repository viewer does not support the HTML5 video tag, a link to the video will be provided instead. Github has difficulty showing video.

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene5-n56k-N42-d0.05-D0.2-mpi4.ogv" type="video/ogg">
<a href="https://kingcountybusinesslaw.com/misc/largescene5-n56k-N42-d0.05-D0.2-mpi4.ogv">[Link to Video]</a>
</video> 

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene4-n55k-N42-D0.15-mpi4.ogv" type="video/ogg">
<a href="https://kingcountybusinesslaw.com/misc/largescene4-n55k-N42-D0.15-mpi4.ogv">[Link to Video</a>
</video> 

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene3-n20k-N30-d.05-D.1.ogv" type="video/ogg">
  <a href="https://kingcountybusinesslaw.com/misc/largescene3-n20k-N30-d.05-D.1.gif">[Link to Video]</a>
</video> 

### Dependencies

This depends on Anonymous1's Linear Algebra Library. (A minimal library for static linear algebra operations, 3D and 4D points, matrices, and transformations). (This can be found in the same set of repositories as this repository. For example, click "GitLab" in the upper left corner if it appears.)

This depends on Tom's Function Library. (A library developed by Tom to provide points, matrices, and transformations as a function of time do that movies can be rendered).

This depends on several other packages. On Ubuntu, these are the following:

`sudo apt install libmpich-dev freeglut3-dev libmagick++-dev`

ImageMagick++ is Used to output frames to .jpg images from which movies can be composited. On Ubuntu, this is currently libmagick++-dev

libglut is used to output frames to the screen. On Ubuntu, the package is freeglut3-dev

If you intend to run this on multiple cores (you should) it requires MPI. On Ubuntu, the package is libmpich-dev

### Install

(After Installing Dependencies)

```
./configure --enable-mpi
make -j 2
sudo make install
```

### Running an example

Examples should be run from the scenes directory.

`cd scenes`

*One process rendering to screen:*

`beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2`

*Four processes rendering to screen:*

`mpirun -n 4 beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2`

If four display windows open, you may be running a process compiled with
g++ instead of mpiCC. Run `make clean` and then configure and install
again, being sure MPI is installed and that you passed --enable-mpi
to configure.

*Two processes rendering one hundred frames to files and the screen:*

`mpirun -n 2 beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2 -F outfilename -f 100`

*Two processes rendering one hundred frames to files only:*

`mpirun -n 2 beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2 -F outfilename -f 100 -S`

### Notes Tuning Parameters

Two big "knobs" are the number of photons used in a scene and the number
of neighboring photons to use in a luminance estimate. The number of photons
is the number of photons thrown out by the light sources, only some of which
will hit an object (unless the light is fully enclosed by surfaces).

**From 1000 to 79000 photons**

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene-ns-40N-mpi4-0.ogv" type="video/ogg">
  <a href="https://kingcountybusinesslaw.com/misc/largescene-ns-40N-mpi4-0.ogv">[Link to Video]</a>
</video> 

This is a video showing an image generated with from 1000 to 79000 photons (1,000 extra in each frame, starting at 1000). It was generated in a for loop with `mpirun -n 4 -s largescene1.dat -n NUMPHOTONS -N 40`

As this video shows, an image created with only a few photons reveals many artifacts of the luminance estimate process, but the image becomes smoother and more photoralistic as the number of photons increases.

**From 1 to 99 Neighbors**

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene-n55k-Ns-mpi4-0.ogv" type="video/ogg">
  <a href="https://kingcountybusinesslaw.com/misc/largescene-n55k-Ns-mpi4-0.gif">[Link to Video]</a>
</video> 

This is an assembly of frames generated with `mpirun -n 4 -s largescene1.dat -n 55000 -N NUMNEIGHBORS`

With only one neighbor, you can tell where individial photons end up on the map. As the video progresses, the number of photons used in the luminance estimate increases and the estimate becomes more accurate.

**Other Parameters**

Other parameters can make an important difference as well. Try playing with minDist and maxDist (set with parameters -d and -D, respectively). You can also control the rate of change in "time" per frame by changing dtdf in the scene files.

Note most scene files in the scenes directory are meant to be run using the scenes directory as the working directory.

### Parallel Execution

Two parts of the algorithms involved are done in parallel. First, the photons to be
emitted are divided among the processes. Then the photon maps are gathered by the
primary process, built into a kd-Tree, and distributed. Finally, the viewport is
parcelled out to child processes one row at a time, as the master process works
on one pixel at a time in between polling to see if it needs to send out new
jobs (rows). Finally, the master process will write the next frame to the GL
window (using glut) and/or the next file (using ImageMagick++).

### A Note

Like most code we try to grow beyond, I simultaneously find this awesome and am embarassed by it. I've learned much more about how to write good code since I first put this together. You can also make amazing images today with simple code, good art, and a modern graphics library behind you. The 500MB g3d library, for example, has a nice photon mapper sample. Still, this was a great project to work on and to parallelize. I developed this for my graphics class, parallellized it with one other student as the final project for our parallel processing class, and worked on it a bit as part of a graphics independent study. After college I continued to pick it up occasionally and do some work on it for fun.
