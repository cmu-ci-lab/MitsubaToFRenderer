function I = exr2avi(file, display, saveAvi, scaling)
% exr2avi(file, display, saveAvi, scaling)
% Inputs:  
% file    -> file name of the input exr file (if saved, the output will have same name as input, with extension replaced by avi)
% display -> display the movie as it is building (defaults to 1)
% saveAvi -> boolean valued, wheather to save the avi file or not (defaults to 0)
% scaling -> equivalent of exposure duration (defaults the scaling to set 95 percentile max value as 255 intensity value)
% Outputs:
% I       -> Image or Video array mxnx3f (3 times number of frames)
% example exr2avi(cbox.exr, 1); show the video or image, don't save, and with auto-scaling
% example exr2avi(cbox.exr, 0, 1, 250); don't show but save and with scaling of 250;

if(nargin == 0)
    disp('file input needed');
    return;
elseif(nargin == 1)
    display = 1;
end
[I, props] = exrread(file);
r = regexp(props.channels,'\d+','match');
R = zeros(length(r),1);

if(~isempty(r{1}))
    for i=1:length(r)
        R(i) = str2num(r{i}{1});
    end

    [~, indices] = sort(R);

    I = I(:,:,indices);
    R = R(indices);
    [~, indices] = sort(R);

    % Swap Blue and Red indices
    [indices(1:3:end), indices(3:3:end)] = deal(indices(3:3:end), indices(1:3:end));

    I = I(:,:,indices);
end

I = double(I);

if(display)
    if(~isempty(r{1}))
        fileName = strrep(file,'exr','avi');
    else
        fileName = strrep(file,'exr','png');
    end
    if(nargin <= 2)
        displayAndSaveVideo( I, fileName); 
    elseif(nargin == 3)
        displayAndSaveVideo( I, fileName, saveAvi);
    elseif(nargin == 4)
        displayAndSaveVideo( I, fileName, saveAvi, scaling );
    end
end

