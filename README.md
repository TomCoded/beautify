Beautify
=====================================================

### Example

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene4-n55k-N42-D0.15-mpi4.ogv" type="video/ogg">
Your browser does not support the video tag.
</video> 

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene3-n20k-N30-d.05-D.1.ogv" type="video/ogg">
Your browser does not support the video tag.
</video> 


This shows an image rendered by throwing 20,000 photons into the scene. Depending on the result of a photon roullete process, they can bounce off objects (being adjusted for the BRDF) and possibly land on other objects, or they can be absorbed. The photons bounce locations (if not absorbed) are stored in a KD-tree. Then rays are shot from the camera into the scene and, if an object is hit, luminance information for that spot on the object is calculated using the nearest N photons (up to 40 in this case). 

### Dependencies


This depends on Anonymous1's Linear Algebra Library.

This depends on Tom's Function Library.

This depends on ImageMagick++

This depends on libglut

If you intend to run this on multiple cores (you should) it requires MPI.

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
Your browser does not support the video tag.
</video> 

This is a video showing an image generated with from 1000 to 79000 photons (1,000 extra in each frame, starting at 1000). It was generated in a for loop with `mpirun -n 4 -s largescene1.dat -n NUMPHOTONS -N 40`

As this video shows, an image created with only a few photons reveals many artifacts of the luminance estimate process, but the image becomes smoother and more photoralistic as the number of photons increases.

**From 1 to 99 Neighbors**

<video controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene-n55k-Ns-mpi4-0.ogv" type="video/ogg">
Your browser does not support the video tag.
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
