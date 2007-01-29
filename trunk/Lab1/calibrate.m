function Xij = calibrate(image, curve)
%   The calibrate function calibrates the image, parameter 'image'
%   using the camera curve, parameter 'curve'.
%	'curve' is the 3-by-256 element vector containing the response curve of the
%	camera used. Can be obtained through the mkhdr program created by
%	Debevec, Hawkins & Duiker (California 1999).
%
%   The return value from the routine is the calibrated image 

for i = 1:size(image, 1)
    for j = 1:size(image, 2)
            
        % Pixel color from picture: Yij
        Yij = image(i, j, :);
        
        % The exposure Xij
        for c = 1:3
            Xij(i, j, c) = curve(Yij(c)+1, c);
        end 
    end
end

% Normalize the values in the picture
%maxValue = max(Xij(:));
%Xij = Xij ./ maxValue;
