function weightFunction = CreateWeightFunction();
min = 10;
max = 240;

weightFunction = zeros(1, 256);
    
for i=min:max
    weightFunction(i) = 1;
end

%calculate slope
x1 = 10;
x2 = 50;
y1 = 0;
y2 = 1;

m = (y2-y1)/(x2-x1);

for i=x1:x2
    weightFunction(i) = (i-x1)*m;
end

x1 = 200;
x2 = 240;
y1 = 1;
y2 = 0;

m = (y2-y1)/(x2-x1);

for i=x1:x2
    weightFunction(i) = 1+(i-x1)*m;
end