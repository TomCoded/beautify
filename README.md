Beautify
=====================================================

### Example

<!-- ![increasing photons with up to a forty-photon sample](largescene-ns-40N-mpi4-0.gif  "From 1000 to 79000 photons") -->

<!--
<video width="320" height="240" controls>
  <source src="https://kingcountybusinesslaw.com/misc/largescene-ns-40N-mpi4-0-html5.mp4" type="video/mp4">
Your browser does not support the video tag.
</video> 

<a href="https://kingcountybusinesslaw.com/tom/gitlist/beautify.git/raw/master/video/largescene-ns-40N-mpi4-0.mp4"><img src="blob/master/largescene-ns-40N-mpi4-0.gif"></a>
-->

<a href="https://kingcountybusinesslaw.com/misc/largescene-ns-40N-mpi4-0.mp4">Here</a> is a sample video that can be played with mpv

It is an assembly of frames generated with mpirun -n 4 -s largescene1.dat -n NUMPHOTONS -N 40.

As this video shows, an image created with only a few photons reveals many artifacts of the luminance estimate process, but the image becomes smoother and more photoralistic as the number of photons increases.

This shows an image rendered by throwing from 1000 to 79000 photons (1,000 extra in each frame, starting at 1000) into the scene. Depending on the result of a photon roullete process, they can bounce off objects (being adjusted for the BRDF) and possibly land on other objects, or they can be absorbed. The photons bounce locations (if not absorbed) are stored in a KD-tree. Then rays are shot from the camera into the scene and, if an object is hit, luminance information for that spot on the object is calculated using the nearest N photons (up to 40 in this case). 

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

*One process rendering to screen*

`beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2`

*Four processes rendering to screen*

`mpirun -n 4 beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2`

*Two processes rendering one hundred frames to files and the screen*

`mpirun -n 2 beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2 -F outfilename -f 100`

*Two processes rendering one hundred frames to files only*

`mpirun -n 2 beautify -s sample.dat -n 3000 -N 20 -d 0.1 -D 0.2 -F outfilename -f 100 -S`

### Notes Tuning Parameters

Two big "knobs" are the number of photons used in a scene and the number
of neighboring photons to use in a luminance estimate. The number of photons
is the number of photons thrown out by the light sources, only some of which
will hit an object (unless the light is fully enclosed by surfaces).

### Parallel Execution

Two parts of the algorithms involved are done in parallel. First, the photons to be
emitted are divided among the processes. Then the photon maps are gathered by the
primary process, built into a kd-Tree, and distributed. Finally, the viewport is
parcelled out to child processes one row at a time, as the master process works
on one pixel at a time in between polling to see if it needs to send out new
jobs (rows). Finally, the master process will write the next frame to the GL
window (using glut) and/or the next file (using ImageMagick++).
