function subR = relabelRegions(subR)

[dx, dy] = size(subR);
subNo = max(max(subR));
newLabels = zeros(subNo, 1);
currLabel = 1;

for i = 1 : dx
    for j = 1 : dy
        if subR(i, j) > 0
            sid = subR(i, j);
            if newLabels(sid) == 0
                newLabels(sid) = currLabel;
                currLabel = currLabel + 1;
            end
        end
    end
end

for i = 1 : dx
    for j = 1 : dy
        if subR(i, j) > 0
            subR(i, j) = newLabels(subR(i, j));
        end
    end
end
subNo = max(max(subR));

end