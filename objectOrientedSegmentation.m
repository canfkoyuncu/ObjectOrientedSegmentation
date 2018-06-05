%-------------------------------------------------------------------------%
%------------- Object Oriented Segmentation of Cell Nuclei ---------------%
%------------------- Fluorescence Microscopy Images ----------------------%
%-------------------------------------------------------------------------%
%
% This work introduces a new nucleus segmentation algorithm that relies on 
% using gradient information not at the pixel-level but at the object-level. 
% To this end, it proposes to decompose an image into smaller homogeneous 
% subregions, define edge-objects at four different orientations to encode 
% the gradient information at the object-level, and devise a merging step, 
% in which the edge-objects vote for subregion pairs along their orientations 
% and the pairs are iteratively merged if they get sufficient votes from 
% multiple orientations.
%   
%   NOTE: The following source codes and executable are provided for 
%   research purposes only. The authors have no responsibility for any 
%   consequences of use of these source codes and the executable. If you 
%   use any part of these codes, please cite the following paper.
%   
%   C. F. Koyuncu, R. C. Atalay, and C. Gunduz-Demir, "Object Oriented 
%   Segmentation of Cell Nuclei in Fluorescence Microscopy Images," 
%   submitted to IEEE Trans. Med. Imag.
%
%
%   If that is YOUR FIRST USE of this program run makeAll file once.
%   This file produces a Matlab executables and adds necessary folders into
%   Matlab path.
%
%   Please contant Can Fahrettin Koyuncu at canfkoyuncu@gmail.com for 
%   further questions.
% 
% 
%   This function takes eight inputs and outputs the segmentation results
%   inputName:  Image filename (an RGB image)
%               -----> Edge-object definition <-----
%   tsize:      Minimum height/width for a component to be an edge-object
%               -----> Sub-region partitioning <-----
%   nslic:      Expected number of superpixels of the SLIC algorithm
%   kslic:      Spatiality constant of the SLIC algorithm for adjusting 
%               the weight of L, a, b values over coordinates in clustering
%               -----> Subregion merging <----- 
%   dmax:       Maximum distance within which an edge-object can vote for 
%               a subregion
%   tvote:      Minimum score that a subregion pair should take from at 
%               least one vertical (left or right) and at least one 
%               horizontal (top or bottom) edge type to be qualified for 
%               merging
%               -----> Postprocessing <----- 
%   tarea:      Minimum area for a subregion to be a nucleus
%   rfilter:    Radius of a majority 
% 
% 
%   Example use:
%       segmRes = objectOrientedSegmentation ('hepg2f_1.jpg', 10, 4000, ...
%                                                      5, 20, 0.1, 350, 3);
% 
%-------------------------------------------------------------------------%
%-------------------------------------------------------------------------%

function segmRes = objectOrientedSegmentation (inputName, tsize, nslic, ...
                                        kslic, dmax, tvote, tarea, rfilter)
    im = imread (inputName);
    [L, ~, ~] = RGB2Lab(im);

% %     mask
    L = L / max(max(L));
    mask = im2bw(L, graythresh(L));

% %     subregions
    [newSlic, ~]    = slicmex(im, nslic, kslic); 
    newSlic         = double(newSlic) + 1;
    newSlic(mask==0)= 0;
    newSlic         = relabelImage(newSlic);

    connectivity = 8;
    d            = 3;
    otsuKs       = 0.5:0.5:2.0;
    rights       = []; lefts        = [];
    tops         = []; bottoms      = [];
    for otsuK = otsuKs
        [right, left, top, bottom] = identifyPrims (L, d, tsize, connectivity, mask, otsuK);
        if otsuK == otsuKs(1)
            newSlic = findSubRegions (newSlic, left, right, top, bottom);
        end

        newSlic = objOrientSegm (newSlic, right, left, top, bottom, tvote, dmax, connectivity, 1, tarea);

        rights  = cat(3, rights, right);
        lefts   = cat(3, lefts, left);
        tops    = cat(3, tops, top);
        bottoms = cat(3, bottoms, bottom);
    end
    segmRes = postProcessing (newSlic, tarea, mask, rights, lefts, tops, bottoms, rfilter);
end

function labeled = relabelImage(map)
    labels = unique(nonzeros(map))';
    label_map = zeros(1, max(labels));
    for label = labels
        ind = find(labels == label);
        label_map(label) = ind(1);
    end
    labeled = zeros(size(map));
    for i=1:size(labeled,1)
        for j=1:size(labeled,2)
            if map(i,j) > 0
                labeled(i,j) = label_map(map(i,j));
            end
        end
    end
end