function writehdr(img, filename, exposure)
% writehdr - write out a floating-point RGB image as a Radiance "Real Pixels" HDR image.
% The data file contains a header followed by raw R,G,B,E bytes, interleaved in that order.
% Usage: writehdr(img, filename, exposure)
% The input image should be a floating point R,G,B image (size width x height x 3).
% The output file is not run-length encoded. It contains only raw 4-byte pixel scanlines.
% The "exposure" argument is optional.

% Extract R, G, B components
dims=size(img);
height=dims(1);
width=dims(2);

R=img(:,:,1);
G=img(:,:,2);
B=img(:,:,3);
% Calculate the exponent for each pixel
E=ceil(log2(max(max(R,G),B)));
E(find(max(max(R,G),B)==0))=0; % 0 represented as 0*2^0
% Normalize pixel values
R=R./(2.^E)*256;
G=G./(2.^E)*256;
B=B./(2.^E)*256;
% Set any pixels that happen to be exactly 1 (256) to 0.5 (128) instead
adjust=(find(max(max(R,G),B))>255);
E(adjust)=E(adjust)+1;
R(adjust)=R(adjust)/2;
G(adjust)=G(adjust)/2;
B(adjust)=B(adjust)/2;
E=E+128;
% Transpose and convert the components to 8-bit unsigned bytes
R=uint8(R');
G=uint8(G');
B=uint8(B');
E=uint8(E');
% Interleave the channels to a linear output array
intl=(1:4:width*height*4);
hdr=uint8(zeros([1,width*height*4]));
hdr(intl)=R(:);
hdr(intl+1)=G(:);
hdr(intl+2)=B(:);
hdr(intl+3)=E(:);
% Write the output file
fid=fopen(filename,'w');
fprintf(fid,'#?RADIANCE\nFORMAT="32-bit_rle_rgbe"\nSOFTWARE="Matlab-generated HDR file"\n');
if(nargin>2)
   fprintf(fid,'EXPOSURE=%d\n',exposure);
end
fprintf(fid,'\n-Y %d +X %d\n',height,width); % Transposed - width is actually height!
fwrite(fid,hdr,'uint8');
fclose(fid);
