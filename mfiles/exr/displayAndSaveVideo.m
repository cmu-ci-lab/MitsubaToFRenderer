function displayAndSaveVideo( I, outFileName, display, saveAvi, scaling, gamma)

if(nargin == 0)
    disp('Input image matrix needed');
elseif(nargin == 1)
    outFileName = 'temp.avi';
    display = 1;
    saveAvi = 0;
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
    gamma   = 1;
elseif(nargin == 2)
    display = 1;
    saveAvi = 0;
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
    gamma   = 1;
elseif(nargin == 3)
    saveAvi = 0;
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
    gamma   = 1;
elseif(nargin == 4)
    scaling = 256/quantile(I(:),0.95); %Allow some saturation
    gamma   = 1;
elseif(nargin == 5)
    gamma   = 1;
end

Frames = size(I,3)/3;
FrameMin = nan;
FrameMax = nan;


ImageRGB = zeros(size(I,1), size(I,2), 3, Frames);
if(display)
figure,
end
for i=1:Frames
   Image = I(:,:,(i-1)*3+1:i*3);
   ImageRGB(:, :, :, i) = uint8((Image*scaling).^gamma);
   if(sum(Image(:))==0)
       continue;
   end
   if(isnan(FrameMin))
       FrameMin = i;
   end
   FrameMax = i;
   
   if(display)
       imshow(uint8( (Image*scaling).^gamma ));
       xlabel(strcat('Frame number:',num2str(i)));
       drawnow;
   end
end
% Uncomment if the video should be played by immovie after display
% if(Frames ~= 1)
%     implay(immovie(ImageRGB));
% end
if(saveAvi)
    if(Frames == 1)
        imwrite(uint8( (Image*scaling).^gamma ), outFileName);
    else
        Time = 10;% Total time of the final video in seconds. 
        v = VideoWriter(outFileName);
        v.FrameRate = (FrameMax - FrameMin)/Time ;v.Quality = 100;
        open(v);   
        for i=FrameMin:FrameMax
            Image = I(:,:,(i-1)*3+1:i*3);
    %         writeVideo(v,uint8(Image*scaling));
            writeVideo(v,uint8( (Image*scaling).^gamma ));
        end
        close(v);
    end
end

end

