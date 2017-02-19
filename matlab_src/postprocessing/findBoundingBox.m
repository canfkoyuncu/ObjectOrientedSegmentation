function bndBox = findBoundingBox(subR)

maxNo = max(max(subR));
[dx, dy] = size(subR);

bndBox = zeros(maxNo, 4);
for i = 1 : dx
    for j = 1 : dy
        if subR(i, j) > 0
            sid = subR(i, j);
            if bndBox(sid, 1) == 0 || bndBox(sid, 1) > i,       bndBox(sid, 1) = i;         end
            if bndBox(sid, 2) == 0 || bndBox(sid, 2) < i,       bndBox(sid, 2) = i;         end
            if bndBox(sid, 3) == 0 || bndBox(sid, 3) > j,       bndBox(sid, 3) = j;         end
            if bndBox(sid, 4) == 0 || bndBox(sid, 4) < j,       bndBox(sid, 4) = j;         end
        end
    end
end

end
