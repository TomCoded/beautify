ffmpeg -y -an -r 10 -i largescene3-n20k-N30-d.05-D.1-%d.jpg -b 2000k largescene3-n20k-N30-d.05-D.1.ogv
ffmpeg -r 10 -v verbose -y -an -i largescene5-n56k-N42-d0.05-D0.2-mpi4-%d.jpg -r 10 -b 2000k largescene5-n56k-N42-d0.05-D0.2-mpi4.ogv
