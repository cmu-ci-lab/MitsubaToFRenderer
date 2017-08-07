function exr2avi(file, saveAvi, scaling)
% exr2avi(file, saveAvi, scaling)
% example exr2avi(cbox.exr, 1); show and save the avi file with auto-scaling
% example exr2avi(cbox.exr, 0, 250); show and dont' save with a scaling of 250;
% file -> file name of the exr file
% saveAvi -> boolean valued, wheather to save the avi file or not
% scaling -> equivalent of exposure duration

if(nargin == 1)
    disp('file input needed');
    return;
end
[I, props] = exrread(file);
r = regexp(props.channels,'\d+','match');
R = zeros(length(r),1);

for i=1:length(r)
    R(i) = str2num(r{i}{1});
end

[~, indices] = sort(R);

I = I(:,:,indices);
R = R(indices);


I = double(I);

if(nargin == 1)
    saveAvi = 0;
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
else if (nargin == 2)
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
    end
end
    

Frames = size(I,3)/3;
FrameMin = nan;
FrameMax = nan;

figure,
for i=1:Frames
   Image = flip(I(:,:,(i-1)*3+1:i*3),3);
   if(sum(Image(:))==0)
       continue;
   end
   if(isnan(FrameMin))
       FrameMin = i;
   end
   FrameMax = i;
   
   imshow(uint8( (Image*scaling).^1 ));
   title(strcat('Frame number:',num2str(i)));
   pause(.2);
end

if(saveAvi)
    Time = 10;% Total time of the final video in seconds. 
    v = VideoWriter(strrep(file,'exr','avi'));
    v.FrameRate = (FrameMax - FrameMin)/Time ;v.Quality = 100;
    open(v);   
    for i=FrameMin:FrameMax
        Image = flip(I(:,:,(i-1)*3+1:i*3),3);
        writeVideo(v,uint8(Image*scaling));
    end
    close(v);
end