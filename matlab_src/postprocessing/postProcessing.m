function res = postProcessing (newSlic, areaThr, mask, rights, lefts, tops, bottoms, rfilter)
    res = eliminateSmalls(newSlic, areaThr);

    leftRes     = assignLeftEdges2Regions(res, mask, lefts);
    rightRes    = assignRightEdges2Regions(res, mask, rights);
    topRes      = assignTopEdges2Regions(res, mask, tops);
    bottomRes   = assignBottomEdges2Regions(res, mask, bottoms);

    res(leftRes > 0)   = leftRes(leftRes > 0);
    res(rightRes > 0)  = rightRes(rightRes > 0);
    res(topRes > 0)    = topRes(topRes > 0);
    res(bottomRes > 0) = bottomRes(bottomRes > 0);
    
    res = modeFilter2D(res, rfilter, 1);
end