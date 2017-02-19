function res = assignBottomEdges2Regions(subR, mask, bottoms)

orgR = subR;
bndBox = findBoundingBox(subR);
labels = zeros(size(bottoms));
for i=1:size(labels,3)
    labels(:,:,i) = labelBottoms(subR, bottoms(:,:,size(labels,3)-i+1), mask);
end

maxNo = max(max(subR));
[dx, dy] = size(subR);
res = zeros(dx, dy);
for k = 1 : maxNo
    if bndBox(k, 1) > 0
            for i=1:size(bottoms,3)
                [res, subR] = assignBottom4UnvisitedLines(subR, bndBox, k, labels(:,:,i), res);
            end
    end
end

res     = eliminateUnconnectedEdges(orgR, res);
others  = connectRegionsWithConnectedBottomEdges(orgR, res, bndBox);
res(others > 0) = others(others > 0);
res(orgR > 0) = orgR(orgR > 0);

end
%--------------------------------------------------------%
%--------------------------------------------------------%
%--------------------------------------------------------%
function others = connectRegionsWithConnectedBottomEdges(orgR, bottom, bndBox)

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
                for ix = i : dx
                    if bottom(ix, j) == k
                        found = 1;
                        break;
                    end
                end
                
                if found == 1
                    for ix = i : dx
                        if bottom(ix, j) == k
                            break;
                        end
                        others(ix, j) = k;
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
function [res, subR] = assignBottom4UnvisitedLines(subR, bndBox, k, labeledBottom, res)

[dx, ~] = size(subR);
for i = bndBox(k, 1) : bndBox(k, 2)
    for j = bndBox(k, 3) : bndBox(k, 4)
        if subR(i, j) == k
            for ix = i : dx
                if labeledBottom(ix, j) == k
                    res(ix, j) = k;
                    for ix2 = i : dx
                        if labeledBottom(ix2, j) ~= k
                            break
                        end
                        res(ix2, j) = k;
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
function labeledBottom = labelBottoms(subR, bottom, mask)

[dx, dy] = size(bottom);
labeledBottom = zeros(dx, dy);
for i = 1 : dx
    for j = 1 : dy
        if bottom(i, j) > 0
            for ix = i : -1 : 1
                if mask(ix, j) == 0
                    break
                end
                if subR(ix, j) > 0
                    labeledBottom(i, j) = subR(ix, j);
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
