function res = assignLeftEdges2Regions(subR, mask, lefts)

orgR = subR;
bndBox = findBoundingBox(subR);

labels = zeros(size(lefts));
for i=1:size(labels,3)
    labels(:,:,i) = labelLefts(subR, lefts(:,:,size(labels,3)-i+1), mask);
end

maxNo = max(max(subR));
[dx, dy] = size(subR);
res = zeros(dx, dy);
for k = 1 : maxNo
    if bndBox(k, 1) > 0
        for i=1:size(lefts,3)
            [res, subR] = assignLeft4UnvisitedLines(subR, bndBox, k, labels(:,:,i), res);
        end
    end
end

res             = eliminateUnconnectedEdges(orgR, res);
others          = connectRegionsWithConnectedLeftEdges(orgR, res, bndBox);
res(others > 0) = others(others > 0);
res(orgR > 0)   = orgR(orgR > 0);
end
%--------------------------------------------------------%
%--------------------------------------------------------%
%--------------------------------------------------------%
function others = connectRegionsWithConnectedLeftEdges(orgR, left, bndBox)

maxNo = max(max(orgR));
[dx, dy] = size(orgR);
others = zeros(dx, dy);

for k = 1 : maxNo
    if bndBox(k, 1) == 0
        continue
    end
    
    for i = bndBox(k, 1) : bndBox(k, 2)
        for j = bndBox(k, 3) : bndBox(k, 4)
            if orgR(i, j) == k
                found = 0;
                for jy = j : -1 : 1
                    if left(i, jy) == k
                        found = 1;
                        break;
                    end
                end
                
                if found == 1
                    for jy = j : -1 : 1
                        if left(i, jy) == k
                            break;
                        end
                        others(i, jy) = k;
                    end
                end
            end
        end
    end
end

end
%--------------------------------------------------------%
%--------------------------------------------------------%
%--------------------------------------------------------%
function [res, subR] = assignLeft4UnvisitedLines(subR, bndBox, k, labeledLeft, res)

for i = bndBox(k, 1) : bndBox(k, 2)
    for j = bndBox(k, 3) : bndBox(k, 4)
        if subR(i, j) == k
            for jy = j : -1 : 1
                if labeledLeft(i, jy) == k
                    res(i, jy) = k;
                    
                    for jy2 = jy : -1 : 1
                        if labeledLeft(i, jy2) ~= k
                            break
                        end
                        res(i, jy2) = k;
                    end
                    
                    subR(i, j) = 0;
                    break
                end
            end
        end
    end
end

end
%--------------------------------------------------------%
%--------------------------------------------------------%
%--------------------------------------------------------%
function labeledLeft = labelLefts(subR, left, mask)

[dx, dy] = size(left);
labeledLeft = zeros(dx, dy);
for i = 1 : dx
    for j = 1 : dy
        if left(i, j) > 0
            for jy = j : dy
                if mask(i, jy) == 0
                    break
                end
                if subR(i, jy) > 0
                    labeledLeft(i, j) = subR(i, jy);
                    break
                end
            end
        end
    end
end

end
%--------------------------------------------------------%
%--------------------------------------------------------%
%--------------------------------------------------------%
