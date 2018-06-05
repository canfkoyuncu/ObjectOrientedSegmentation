function res = modeFilter2D(map, radius, ignoreBg)
	res = zeros(size(map));
	maxLabel = max(max(map))+1;
    for i=1:size(map,1)
        for j=1:size(map,2)
            if ignoreBg == 1 && map(i,j) == 0
				res(i,j) = 0;
            else
				values = zeros(1,maxLabel);
                for i2=-(radius-1):(radius-1)
                    for j2=-(radius-1):(radius-1)
                        if (i2*i2 + j2*j2) <= radius*radius
							r = i + i2;
							c = j + j2;
                            if r >= 1 && c >= 1 && r <= size(map,1) && c <= size(map,2)
								values(map(r,c)+1) = values(map(r,c)+1) + 1;
                            end
                        end
                    end
                end
				maxInd = 1;
				maxVal = values(1);
                for k=2:maxLabel
                    if values(k) > maxVal
						maxVal = values(k);
						maxInd = k;
                    end
                end

				res(i,j) = maxInd-1;
            end
        end
    end
end