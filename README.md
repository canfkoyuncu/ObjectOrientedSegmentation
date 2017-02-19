# Object Oriented Segmentation of Cell Nuclei in Fluorescence Microscopy Images

This work introduces a new nucleus segmentation algorithm that relies on using gradient information not at the pixel-level but at the object-level. To this end, it proposes to decompose an image into smaller homogeneous subregions, define edge-objects at four different orientations to encode the gradient information at the object-level, and devise a merging step, in which the edge-objects vote for subregion pairs along their orientations and the pairs are iteratively merged if they get sufficient votes from multiple orientations.

NOTE: The following source codes and executable are provided for research purposes only. The authors have no responsibility for any consequences of use of these source codes and the executable. If you use any part of these codes, please cite the following paper.
>C. F. Koyuncu, R. C. Atalay, and C. Gunduz-Demir, "Object Oriented Segmentation of Cell Nuclei in Fluorescence Microscopy Images," submitted to IEEE Trans. Med. Imag.

If that is YOUR FIRST USE of this program run `makeAll` file once. This file produces a Matlab executables and adds necessary folders into Matlab path.

Please contant Can Fahrettin Koyuncu at canfkoyuncu@gmail.com for further questions.

The function `objectOrientedSegmentation` takes eight inputs and outputs the segmentation results:
* ***inputName***:  Image filename (an RGB image)
* **Edge-object definition**
  * ***tsize***:      Minimum height/width for a component to be an edge-object
* **Sub-region partitioning**
  * ***nslic***:      Expected number of superpixels of the SLIC algorithm
  * ***kslic***:      Spatiality constant of the SLIC algorithm for adjusting the weight of L, a, b values over coordinates in clustering
* **Subregion merging**
  * ***dmax***:       Maximum distance within which an edge-object can vote for a subregion
  * ***tvote***:      Minimum score that a subregion pair should take from at least one vertical (left or right) and at least one horizontal (top or bottom) edge type to be qualified for merging
* **Postprocessing**
  * ***tarea***:      Minimum area for a subregion to be a nucleus
  * ***rfilter***:    Radius of a majority 

Example use:
      ```segmRes = objectOrientedSegmentation ('hepg2f_1.jpg', 10, 4000, 5, 20, 0.1, 350, 3);```
