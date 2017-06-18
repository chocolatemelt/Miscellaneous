Mandelbrot PPM Generator
========================

## Project Description
This project uses MPI ranks split by threads to handle the computation, generation and I/O of a massive Mandelbrot ppm
(131072 x 131072).

(little side note: this does not actually print a correct mandelbrot set, and I'm too busy to figure out why. however, the actual parallelization looks to be in working order so that's fine)

## Parallel Design
This project requires MPI ranks that are evenly divisible by
the ```g_rankrows``` input. ```g_rankrows``` defines the number
of rows handled by MPI ranks, such that the entire Mandelbrot set
is handled in a grid-like fashion by MPI ranks. For example, a
world handled by 4 MPI ranks with 2 rows would be split up like such:

```
----|----
|   |   |
|   |   |
----|----
|   |   |
|   |   |
----|----
```

Thus, the world would be handled by 4 ranks split up in such a fashion, whereas the same number of ranks with only 1 row would
be split up as such:

```
---|--|--|---
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
---|--|--|---
```

## Studies
The main study here searches for the ideal parameters to generate a Mandelbrot set with an image resolution of 131072x131072. These
parameters include the number of MPI ranks, and the number of rows for those ranks.

A secondary study investigates the effectiveness of splitting up
the world into rows versus simply splitting up into smaller columns.
