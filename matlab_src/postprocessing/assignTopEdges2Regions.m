function res = assignTopEdges2Regions(subR, mask, tops)

orgR = subR;
bndBox = findBoundingBox(subR);
labels = zeros(size(tops));
for i=1:size(labels,3)
    labels(:,:,i) = labelTops(subR, tops(:,:,size(labels,3)-i+1), mask);
end

maxNo = max(max(subR));
[dx, dy] = size(subR);
res = zeros(dx, dy);
for k = 1 : maxNo
    if bndBox(k, 1) > 0
        if bndBox(k, 1) > 0
            for i=1:size(tops,3)
                [res, subR] = assignTop4UnvisitedLines(subR, bndBox, k, labels(:,:,i), res);
            end
        end
    end
end

res     = eliminateUnconnectedEdges(orgR, res);
others  = connectRegionsWithConnectedTopEdges(orgR, res, bndBox);
res(others > 0) = others(others > 0);
res(orgR > 0) = orgR(orgR > 0);

end
%--------------------------------------------------------%
%--------------------------------------------------------%
%--------------------------------------------------------%
function others = connectRegionsWithConnectedTopEdges(orgR, top, bndBox)

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
                for ix = i : -1 : 1
                    if top(ix, j) == k
                        found = 1;
                        break;
                    end
                end
                
                if found == 1
                    for ix = i : -1 : 1
                        if top(ix, j) == k
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
function [res, subR] = assignTop4UnvisitedLines(subR, bndBox, k, labeledTop, res)

for i = bndBox(k, 1) : bndBox(k, 2)
    for j = bndBox(k, 3) : bndBox(k, 4)
        if subR(i, j) == k
            for ix = i : -1 : 1
                if labeledTop(ix, j) == k
                    res(ix, j) = k;
                    for ix2 = ix : -1 : 1
                        if labeledTop(ix2, j) ~= k
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
function labeledTop = labelTops(subR, top, mask)

[dx, dy] = size(top);
labeledTop = zeros(dx, dy);
for i = 1 : dx
    for j = 1 : dy
        if top(i, j) > 0
            for ix = i : dx
                if mask(ix, j) == 0
                    break
                end
                if subR(ix, j) > 0
                    labeledTop(i, j) = subR(ix, j);
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
