function res = assignRightEdges2Regions(subR, mask, rights)

orgR = subR;
bndBox = findBoundingBox(subR);
labels = zeros(size(rights));
for i=1:size(labels,3)
    labels(:,:,i) = labelRights(subR, rights(:,:,size(labels,3)-i+1), mask);
end

maxNo = max(max(subR));
[dx, dy] = size(subR);
res = zeros(dx, dy);
for k = 1 : maxNo
    if bndBox(k, 1) > 0
        for i=1:size(rights,3)
            [res, subR] = assignRight4UnvisitedLines(subR, bndBox, k, labels(:,:,i), res);
        end
    end
end
res     = eliminateUnconnectedEdges(orgR, res);
others  = connectRegionsWithConnectedRightEdges(orgR, res, bndBox);
res(others > 0) = others(others > 0);
res(orgR > 0) = orgR(orgR > 0);

end
%--------------------------------------------------------%
%--------------------------------------------------------%
%--------------------------------------------------------%
function others = connectRegionsWithConnectedRightEdges(orgR, right, bndBox)

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
                for jy = j : dy
                    if right(i, jy) == k
                        found = 1;
                        break;
                    end
                end
                
                if found == 1
                    for jy = j : dy
                        if right(i, jy) == k
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
function [res, subR] = assignRight4UnvisitedLines(subR, bndBox, k, labeledRight, res)

[~, dy] = size(subR);
for i = bndBox(k, 1) : bndBox(k, 2)
    for j = bndBox(k, 3) : bndBox(k, 4)
        if subR(i, j) == k
            for jy = j : dy
                if labeledRight(i, jy) == k
                    res(i, jy) = k;
                    
                    for jy2 = jy : dy
                        if labeledRight(i, jy2) ~= k
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
function labeledRight = labelRights(subR, right, mask)

[dx, dy] = size(right);
labeledRight = zeros(dx, dy);
for i = 1 : dx
    for j = 1 : dy
        if right(i, j) > 0
            for jy = j : -1 : 1
                if mask(i, jy) == 0
                    break
                end
                if subR(i, jy) > 0
                    labeledRight(i, j) = subR(i, jy);
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
