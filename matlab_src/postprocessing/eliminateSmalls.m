function res = eliminateSmalls(res, smallThr)

[dx, dy] = size(res);
rno = max(max(res));
areas = zeros(rno, 1);
for i = 1 : dx
    for j = 1 : dy
        if res(i,j) > 0
            areas(res(i,j)) = areas(res(i,j)) + 1;
        end
    end
end

for i = 1 : rno
    if areas(i) < smallThr
        areas(i) = 0;
    end
end

for i = 1 : dx
    for j = 1 : dy
        if res(i,j) > 0 && areas(res(i,j)) == 0
            res(i, j) = 0;
        end
    end
end

res = relabelRegions(res);

end
