function displayAndSaveVideo( I, outFileName, saveAvi, scaling )

if(nargin == 0)
    disp('Input image matrix needed');
elseif(nargin == 1)
    outFileName = 'temp.avi';
    saveAvi = 0;
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
elseif(nargin == 2)
    saveAvi = 0;
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
elseif (nargin == 3)
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
end

Frames = size(I,3)/3;
FrameMin = nan;
FrameMax = nan;


ImageRGB = zeros(size(I,1), size(I,2), 3, Frames);
figure,
for i=1:Frames
   Image = I(:,:,(i-1)*3+1:i*3);
   ImageRGB(:, :, :, i) = uint8(Image*scaling);
   if(sum(Image(:))==0)
       continue;
   end
   if(isnan(FrameMin))
       FrameMin = i;
   end
   FrameMax = i;
   
   imshow(uint8( (Image*scaling).^1 ));
   xlabel(strcat('Frame number:',num2str(i)));
   pause(.2);
end
% Uncomment if the video should be played by immovie after display
% if(Frames ~= 1)
%     implay(immovie(ImageRGB));
% end

if(saveAvi)
    if(Frames == 1)
        imwrite(uint8( (Image*scaling).^1 ), outFileName);
    else
        Time = 10;% Total time of the final video in seconds. 
        v = VideoWriter(outFileName);
        v.FrameRate = (FrameMax - FrameMin)/Time ;v.Quality = 100;
        open(v);   
        for i=FrameMin:FrameMax
            Image = I(:,:,(i-1)*3+1:i*3);
    %         writeVideo(v,uint8(Image*scaling));
            writeVideo(v,uint8( (Image*scaling).^1 ));
        end
        close(v);
    end
end

end

