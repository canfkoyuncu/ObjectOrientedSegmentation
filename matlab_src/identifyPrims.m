function [right, left, top, bottom] = identifyPrims (gray, thickness, lengtTh, connectivity, mask, tg)
    [right, left, top, bottom] = findPrims (gray, tg, mask, thickness);

    right(mask==0)  = 0;
    left(mask==0)   = 0;
    top(mask==0)    = 0;
    bottom(mask==0) = 0;
    
%     [right, left, top, bottom] = takeOuterPixels (double(right), double(left), double(top), double(bottom), thickness);
    
	right = takeOneSideBoundary(right, 1, thickness);
	left = takeOneSideBoundary(left, 2, thickness);
	top = takeOneSideBoundary(top, 3, thickness);
	bottom = takeOneSideBoundary(bottom, 4, thickness);
    
    [right,     nx2] = bwlabeln(right, connectivity);
    [left,      nx1] = bwlabeln(left, connectivity);
    [top,       ny2] = bwlabeln(top, connectivity);
    [bottom,    ny1] = bwlabeln(bottom, connectivity);
    
    right   = elimSmallPrims (right, lengtTh, 1);
    left    = elimSmallPrims (left, lengtTh, 1);
    top     = elimSmallPrims (top, lengtTh, 2);
    bottom  = elimSmallPrims (bottom, lengtTh, 2);
end

function [right, left, top, bottom] = findPrims (gray, tg, mask, d)
    [Gx, Gy] = imgradientxy (gray, 'sobel');
    
    leftG = Gx;
    leftG(leftG<0) = 0;
    
    rightG = -Gx;
    rightG(rightG<0) = 0;
    
    topG = Gy;
    topG(topG<0) = 0;
    
    bottomG = -Gy;
    bottomG(bottomG<0) = 0;
    
    tgR = tg*graythresh(rightG);
%     if tgR > 1, tgR = 1; end
    right = im2bw(rightG, tgR);
    
    tgL = tg*graythresh(leftG);
%     if tgL > 1, tgL = 1; end
    left = im2bw(leftG, tgL);
    
    tgT = tg*graythresh(topG);
%     if tgT > 1, tgT = 1; end;
    top = im2bw(topG, tgT);
    
    tgB = tg*graythresh(bottomG);
%     if tgB > 1, tgB = 1; end
    bottom = im2bw(bottomG, tgB);
    
    maskRight = mask;
    maskRight(:, 1:end-d) =  0;
    right = right | maskRight;
    
    maskLeft = mask;
    maskLeft(:, d+1:end) =  0;
    left = left | maskLeft;
    
    maskTop = mask;
    maskTop(d+1:end, :) =  0;
    top = top | maskTop;
    
    maskBottom = mask;
    maskBottom(1:end-d, :) =  0;
    bottom = bottom | maskBottom;
end

function prims = elimSmallPrims (prims, th, side)
    stats = regionprops(prims, 'BoundingBox');
    for i = 1:length(stats)
        t = stats(i).BoundingBox;
        if side == 1 && t(4) < th %% left or right prims
            prims(prims == i) = 0;
        elseif side == 2 && t(3) < th %% top or bottom prims
            prims(prims == i) = 0;
        end
    end
end

function processed = takeOneSideBoundary (prims, sideFlag, thickness)
    processed = zeros(size(prims));

    for i=1:size(prims,1)
        for j=1:size(prims,2)
            if prims(i,j) > 0
                isBorder = 0;
                for k=0:thickness
                    if sideFlag == 1 % Right
                        i2 = i;
                        j2 = j + k;
                    elseif sideFlag == 2 % Left
                        i2 = i;
                        j2 = j - k;
                        
                    elseif sideFlag == 3 % Top
                        i2 = i - k;
                        j2 = j;
                    else
                        i2 = i + k;
                        j2 = j;
                    end
                    if sideFlag == 1 && j2 > size(prims,2)
                        isBorder = 1;
                        break;
                    end
                    if sideFlag == 2 && j2 < 1
                        isBorder = 1;
                        break;
                    end
                    if sideFlag == 3 && i2 < 1
                        isBorder = 1;
                        break;
                    end
                    if sideFlag == 4 && i2 >= size(prims,1)
                        isBorder = 1;
                        break;
                    end
                    if prims(i2,j2) ~= prims(i,j)
                        isBorder = 1;
                        break;
                    end
                end
                if isBorder == 1
                    processed(i,j) = prims(i,j);
                else
                    processed(i,j) = 0;
                end
            else
                processed(i,j) = 0;
            end
        end
    end
end