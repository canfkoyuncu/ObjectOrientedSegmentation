% Internal choices
% 1.  Exclude the edge pixels from the subregion and then find connected
%     components using ------BWLABELN(excl, 4)------
% 2.  Find the right connected components after merging them with the edge
%     pixels using ------BWLABELN(excl, 8)------
% 3.  Find the left connected components using ------BWLABELN(excl, 8)------

function subR = findSubRegions (subR, leftObjects, rightObjects, topObjects, bottomObjects)
    sno = max(max(subR));
    [subR, subNo] = relabelSubRegions(subR, sno);
    subBox = findBoundingBox(subR, subNo);
    [subR, subNo] = splitWithEdgeObjects(subR, subNo, subBox, leftObjects, 1);
    subBox = findBoundingBox(subR, subNo);
    [subR, subNo] = splitWithEdgeObjects(subR, subNo, subBox, rightObjects, 2);
    subBox = findBoundingBox(subR, subNo);
    [subR, subNo] = splitWithEdgeObjects(subR, subNo, subBox, topObjects, 3);
    subBox = findBoundingBox(subR, subNo);
    [subR, subNo] = splitWithEdgeObjects(subR, subNo, subBox, bottomObjects, 4);
end


%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [subRegions, subNo] = splitWithEdgeObjects(subRegions, subNo, subBox, edgeObjects, edgeType)

[subIds, objectIds] = findSplitPairs(subRegions, edgeObjects);

pairNo = length(subIds);
i = 1;
while i <= pairNo
    
    subId = subIds(i);
    objId = objectIds(i);
    
    % Exclude edge pixels, find connected components, if only one DO NOTHING
    [subPix, objPix, connComp, connNo] = cropRegions(subRegions, subId, subBox, edgeObjects, objId);
    
    if connNo > 1
        
        % Find left, right, both, none components
        if edgeType == 1
            selectionMask = markLeftComponents(connComp, connNo, objPix); 
        elseif edgeType == 2
            selectionMask = markRightComponents(connComp, connNo, objPix);
        elseif edgeType == 3
            selectionMask = markTopComponents(connComp, connNo, objPix);
        elseif edgeType == 4
            selectionMask = markBottomComponents(connComp, connNo, objPix);
        end
        
        [newMap, newNo] = findSplittedComponents(connComp, connNo, selectionMask, (objPix & subPix));
        % The original id is used by the first component
        % The other components take the new ids 
        % The bounding box of all of the components remains the same
        % (although the bounding box will not be tightest any more)
        % The overlapping pair list will be updated
        % (although some may not be actually overlapping)
        [subRegions, subNo, subBox, subIds, objectIds, pairNo] = updateAfterSplit(i, newMap, newNo, subRegions, subNo, subBox, subIds, objectIds, pairNo);
    end
    
    i = i + 1;
end
    
end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
% ONLY FOR BOTTOM EDGE-OBJECT TYPE            %
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function isBottom = markBottomComponents(connComp, connNo, bottomPix)

[cx, cy] = size(connComp);
bottomX = zeros(cy, 1);

for y = 1 : cy
    for x = cx : -1 : 1
        if bottomPix(x, y) == 1
            bottomX(y) = x;
            break;
        end
    end
end

isBottom = zeros(connNo, 1);
for y = 1 : cy
    if bottomX(y) > 0
        for x= bottomX(y) + 1 : cx
            if connComp(x, y) > 0 % at least one pixel is found on the right
                isBottom( connComp(x, y) ) = 1;
            end
        end
    end
end

isTop = zeros(connNo, 1);
for y = 1 : cy
    if bottomX(y) > 0
        for x = 1 : bottomX(y)
            if connComp(x, y) > 0 % at least one pixel is found on the top
                isTop( connComp(x, y) ) = 1;
            end
        end
    end
end

for i = 1 : connNo
    if     isBottom(i) == 1 && isTop(i) == 1,  isBottom(i) = 2;
    elseif isBottom(i) == 0 && isTop(i) == 0,  isBottom(i) = 3;
    %elseif isBottom(i) == 1 && isTop(i) == 0, isBottom(i) = 1;   
    %elseif isBottom(i) == 0 && isTop(i) == 1, isBottom(i) = 0;
    end
end

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
% ONLY FOR TOP EDGE-OBJECT TYPE               %
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function isTop = markTopComponents(connComp, connNo, topPix)

[cx, cy] = size(connComp);
topX = zeros(cy, 1);

for y = 1 : cy
    for x = 1 : cx
        if topPix(x, y) == 1
            topX(y) = x;
            break;
        end
    end
end

isTop = zeros(connNo, 1);
for y = 1 : cy
    if topX(y) > 0
        for x = 1 : topX(y) - 1
            if connComp(x, y) > 0 % at least one pixel is found on the top
                isTop( connComp(x, y) ) = 1;
            end
        end
    end
end

isBottom = zeros(connNo, 1);
for y = 1 : cy
    if topX(y) > 0
        for x = topX(y) : cx
            if connComp(x, y) > 0 % at least one pixel is found on the bottom
                isBottom( connComp(x, y) ) = 1;
            end
        end
    end
end

for i = 1 : connNo
    if     isTop(i) == 1 && isBottom(i) == 1,  isTop(i) = 2;
    elseif isTop(i) == 0 && isBottom(i) == 0,  isTop(i) = 3;
    %elseif isTop(i) == 1 && isBottom(i) == 0, isTop(i) = 1;   
    %elseif isTop(i) == 0 && isBottom(i) == 1, isTop(i) = 0;
    end
end

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
% ONLY FOR LEFT EDGE-OBJECT TYPE              %
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function isLeft = markLeftComponents(connComp, connNo, leftPix)

[cx, cy] = size(connComp);
leftY = zeros(cx, 1);

for x = 1 : cx
    for y = 1 : cy
        if leftPix(x, y) == 1
            leftY(x) = y;
            break;
        end
    end
end

isLeft = zeros(connNo, 1);
for x = 1 : cx
    if leftY(x) > 0
        for y = 1 : leftY(x) - 1
            if connComp(x, y) > 0 % at least one pixel is found on the left
                isLeft( connComp(x, y) ) = 1;
            end
        end
    end
end

isRight = zeros(connNo, 1);
for x = 1 : cx
    if leftY(x) > 0
        for y = leftY(x) : cy
            if connComp(x, y) > 0 % at least one pixel is found on the right
                isRight( connComp(x, y) ) = 1;
            end
        end
    end
end

for i = 1 : connNo
    if     isLeft(i) == 1 && isRight(i) == 1,  isLeft(i) = 2;
    elseif isLeft(i) == 0 && isRight(i) == 0,  isLeft(i) = 3;
    %elseif isLeft(i) == 1 && isRight(i) == 0, isLeft(i) = 1;   
    %elseif isLeft(i) == 0 && isRight(i) == 1, isLeft(i) = 0;
    end
end

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
% ONLY FOR RIGHT EDGE-OBJECT TYPE              %
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
% Components on only the left side of the edge-object --> 0
% Components on only the right side of the edge-object --> 1
% Components on both sides of the edge-object --> 2
% Components on neither sid of the edge-object --> 3
function isRight = markRightComponents(connComp, connNo, rightPix)

[cx, cy] = size(connComp);
rightY = zeros(cx, 1);

for x = 1 : cx
    for y = cy : -1 : 1
        if rightPix(x, y) == 1
            rightY(x) = y;
            break;
        end
    end
end

isRight = zeros(connNo, 1);
for x = 1 : cx
    if rightY(x) > 0
        for y = rightY(x) + 1 : cy
            if connComp(x, y) > 0 % at least one pixel is found on the right
                isRight( connComp(x, y) ) = 1;
            end
        end
    end
end

isLeft = zeros(connNo, 1);
for x = 1 : cx
    if rightY(x) > 0
        for y = 1 : rightY(x)
            if connComp(x, y) > 0 % at least one pixel is found on the left
                isLeft( connComp(x, y) ) = 1;
            end
        end
    end
end

for i = 1 : connNo
    if     isRight(i) == 1 && isLeft(i) == 1,  isRight(i) = 2;
    elseif isRight(i) == 0 && isLeft(i) == 0,  isRight(i) = 3;
    %elseif isRight(i) == 1 && isLeft(i) == 0,  isRight(i) = 1;   
    %elseif isRight(i) == 0 && isLeft(i) == 1,   isRight(i) = 0;
    end
end

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
% THIS ARE THE FUNCTIONS THAT CAN BE USED FOR %
% ALL EDGE OBJECTS REGARDLESS OF THEIR TYPES  %
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [subRegions, subNo, subBox, subIds, objIds, pairNo] = updateAfterSplit(currInd, newMap, newNo, subRegions, subNo, subBox, subIds, objIds, pairNo)

subId = subIds(currInd);

for j = 2 : newNo
    subNo = subNo + 1;
    newId = subNo;
    
    newMap(newMap == j) = newId;
    subBox(newId,:) = subBox(subId,:);
    
    for k = currInd + 1 : pairNo
        if subIds(k) == subId
            pairNo = pairNo + 1;
            subIds(pairNo) = newId;
            objIds(pairNo) = objIds(k);
        end
    end
end

newMap(newMap == 1) = subId;

tmp = subRegions(subBox(subId,1):subBox(subId,2), subBox(subId,3):subBox(subId,4));
tmp(tmp == subId) = newMap(tmp == subId);
subRegions(subBox(subId,1):subBox(subId,2), subBox(subId,3):subBox(subId,4)) = tmp;

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [newMap, newNo] = findSplittedComponents(connComp, connNo, selectionMask, objPixMask)
        
[zeroMap, oneMap, twoMap, threeMap] = splitMapIntoFour(connComp, selectionMask);
[zeroNo, oneNo, twoNo, threeNo] = countComponentsOfEachType(selectionMask, connNo);
        
if zeroNo > 0
    zeroMap = zeroMap | objPixMask;
	[newMap, newNo] = bwlabeln(zeroMap, 8);
            
	if twoNo > 0
        [tmpMap, tmpNo] = bwlabeln(twoMap, 8);
        newMap(tmpMap > 0) = tmpMap(tmpMap > 0) + newNo;
        newNo = newNo + tmpNo;
	end
            
elseif twoNo > 0
    twoMap = twoMap | objPixMask;
	[newMap, newNo] = bwlabeln(twoMap, 8);

else
	[newMap, newNo] = bwlabeln(objPixMask, 8);
end


if oneNo > 0
	[tmpMap, tmpNo] = bwlabeln(oneMap, 8);
    newMap(tmpMap > 0) = tmpMap(tmpMap > 0) + newNo;
    newNo = newNo + tmpNo;
end


if threeNo > 0
    [tmpMap, tmpNo] = bwlabeln(threeMap, 8);
    newMap(tmpMap > 0) = tmpMap(tmpMap > 0) + newNo;
    newNo = newNo + tmpNo;
end
                
end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [zeroMap, oneMap, twoMap, threeMap] = splitMapIntoFour(connComp, splitArray)

[dx, dy] = size(connComp);
zeroMap = zeros(dx, dy);
oneMap = zeros(dx, dy);
twoMap = zeros(dx, dy);
threeMap = zeros(dx, dy);

for i = 1 : dx
    for j = 1 : dy
        if connComp(i, j) > 0
            cid = connComp(i, j);
            
            if splitArray(cid) == 0
                zeroMap(i, j) = 1;
            elseif splitArray(cid) == 1
                oneMap(i, j) = 1;
            elseif splitArray(cid) == 2
                twoMap(i, j) = 1;
            else
                threeMap(i, j) = 1;
            end
        end
    end
end

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [zeroNo, oneNo, twoNo, threeNo] = countComponentsOfEachType(selectionMask, len)
    
zeroNo = 0;
oneNo = 0;
twoNo = 0;
threeNo = 0;
    
for i = 1 : len
    if selectionMask(i) == 0
        zeroNo = zeroNo + 1;
	elseif selectionMask(i) == 1
        oneNo = oneNo + 1;
    elseif selectionMask(i) == 2
        twoNo = twoNo + 1;
    elseif selectionMask(i) == 3
        threeNo = threeNo + 1;
    end
end
    
end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [subPix, objPix, connComp, connNo] = cropRegions(subRegions, subId, subBox, edgeObjects, objId)

subPix = subRegions(subBox(subId,1):subBox(subId,2), subBox(subId,3):subBox(subId,4));
subPix = subPix == subId;
    
objPix = edgeObjects(subBox(subId,1):subBox(subId,2), subBox(subId,3):subBox(subId,4));
objPix = objPix == objId;
    
% Exclude the edge-object pixels and find connected components
excl = subPix == 1 & objPix == 0;
[connComp, connNo] = bwlabeln(excl, 4);
    
end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [subIds, objectIds] = findSplitPairs(subRegions, edgeObjects)

subNo     = max(max(subRegions));
objectNo  = max(max(edgeObjects));
whoSplits = zeros(subNo, objectNo);
[dx, dy]  = size(subRegions);

for i = 1 : dx
    for j = 1 : dy
        if subRegions(i, j) > 0 && edgeObjects(i, j) > 0
            whoSplits(subRegions(i, j), edgeObjects(i, j)) = 1;
        end
    end
end
[subIds, objectIds] = find(whoSplits==1);

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function bndBox = findBoundingBox(reg, rno)

[dx, dy] = size(reg);
minx = zeros(rno, 1) + dx + dy;
maxx = zeros(rno, 1);
miny = zeros(rno, 1) + dx + dy;
maxy = zeros(rno, 1);

for i = 1 : dx
    for j = 1 : dy
        if reg(i, j) > 0
            rid = reg(i, j);
            
            if minx(rid) > i,       minx(rid) = i;      end
            if maxx(rid) < i,       maxx(rid) = i;      end
            if miny(rid) > j,       miny(rid) = j;      end
            if maxy(rid) < j,       maxy(rid) = j;      end
        end
    end
end

bndBox(:, 1) = minx;
bndBox(:, 2) = maxx;
bndBox(:, 3) = miny;
bndBox(:, 4) = maxy;

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function [subR, subNo] = relabelSubRegions(subR, subNo)

[dx, dy] = size(subR);
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
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
function subR = eliminateSubRegionsMostlyOutside(subR, subNo, mask)

[dx, dy] = size(subR);
maskA = zeros(subNo,1);
nonA  = zeros(subNo,1);
for i = 1 : dx
    for j = 1 : dy
        sid = subR(i, j);
        if mask(i, j) == 1
            maskA(sid) = maskA(sid) + 1;
        else
            nonA(sid) = nonA(sid) + 1;
        end
    end
end

subEliminate = zeros(subNo, 1);
for i = 1 : subNo
    if nonA(i) > maskA(i)
        subEliminate(i) = 1;
    end
end

for i = 1 : dx
    for j = 1 : dy
        sid = subR(i, j);
        if subEliminate(sid) == 1
            subR(i, j) = 0;
        end
    end
end

end
%---------------------------------------------%
%---------------------------------------------%
%---------------------------------------------%
