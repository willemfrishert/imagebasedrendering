function weight = CalculateWeight(Yij, weightFunction);
%receive RGB tuple, returns weight tuple

weight(1) = weightFunction(Yij(1)+1);
weight(2) = weightFunction(Yij(2)+1);
weight(3) = weightFunction(Yij(3)+1);
