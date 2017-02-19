% edges are labeled with the ids of subregions that they belong to
function edges = eliminateUnconnectedEdges(subR, edges)
maxNo = max(max(edges));
[dx, dy] = size(edges);
bndBox = findBoundingBox(edges);
for k = 1 : maxNo
    if bndBox(k, 1) == 0
        continue
    end
    
    if bndBox(k, 1) > 1,    bndBox(k, 1) = bndBox(k, 1) - 1;    end
    if bndBox(k, 2) < dx,   bndBox(k, 2) = bndBox(k, 2) + 1;    end
    if bndBox(k, 3) > 1,    bndBox(k, 3) = bndBox(k, 3) - 1;    end
    if bndBox(k, 4) < dy,   bndBox(k, 4) = bndBox(k, 4) + 1;    end
end

for k = 1 : maxNo
    if bndBox(k, 1) == 0
        continue
    end
    
    minx = bndBox(k, 1);
    maxx = bndBox(k, 2);
    miny = bndBox(k, 3);
    maxy = bndBox(k, 4);
    
    tempEdges = edges(minx : maxx, miny : maxy);
    tempSub = subR(minx : maxx, miny : maxy);
    [cc, cno] = bwlabeln(tempEdges == k, 8);
    
    [cx, cy] = size(cc);
    isN =  zeros(cno, 1);
    
    for i = 1 : cx
        for j = 1 : cy
            if cc(i, j) > 0
                cid = cc(i, j);
                if i > 1  &&            tempSub(i - 1, j) == k,        isN(cid) = 1;       end
                if i < cx &&            tempSub(i + 1, j) == k,        isN(cid) = 1;       end
                if j > 1  &&            tempSub(i, j - 1) == k,        isN(cid) = 1;       end
                if j < cy &&            tempSub(i, j + 1) == k,        isN(cid) = 1;       end
                if i > 1  && j > 1  &&  tempSub(i - 1, j - 1) == k,    isN(cid) = 1;       end
                if i > 1  && j < cy &&  tempSub(i - 1, j + 1) == k,    isN(cid) = 1;       end
                if i < cx && j > 1  &&  tempSub(i + 1, j - 1) == k,    isN(cid) = 1;       end
                if i < cx && j < cy &&  tempSub(i + 1, j + 1) == k,    isN(cid) = 1;       end
            end
        end
    end
    
    for i = 1 : cx
        for j = 1 : cy
            if cc(i, j) > 0 && isN(cc(i, j)) == 0
                edges(minx + i - 1, miny + j - 1) = 0;
            end
        end
    end
    
end

end

function isN = isNeighbor (tempSub, i, j, maxDist, side, k)
    if side == 2 %% right
        for i2=0:maxDist
            if j-i2 > 0 && tempSub(i,j-i2) == k
                isN = 1;
                return;
            end
        end
    elseif side == 1 %% left
        for i2=0:maxDist
            if j+i2 <= size(tempSub,2) && tempSub(i,j+i2) == k
                isN = 1;
                return;
            end
        end
    elseif side == 4 %% bottom
        for i2=0:maxDist
            if i-i2 > 0 && tempSub(i-i2,j) == k
                isN = 1;
                return;
            end
        end
    elseif side == 3 %% bottom
        for i2=0:maxDist
            if i+i2 <= size(tempSub,1) && tempSub(i+i2,j) == k
                isN = 1;
                return;
            end
        end
    end
    isN = 0;
end