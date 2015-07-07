% This is a demo explaining how to obtain self-similarity hypercubes for 
%   - an entire image
%   - a set of windows in that image
% and how to apply sliding window object detection on that image

addpath(genpath('supportfunctions'))
exampleImageFN='exampleimages/Ssim_02_a.jpg';

I=td_imrescalemaxsize(im2double(imread(exampleImageFN)),200, 100);
% I=im2double(imread(exampleImageFN));
H=size(I,1);
W=size(I,2);

% instantiate self-similarity object and define settings
SS=td_fastSelfSim;

SS.numberClusters=400;    % number of clusters for the patch prototype codebook
SS.clusteringMethod=1;    % random subsampling of the patches to be clustered
SS.clusteringPatchesNumber=20000;  % number of patches to be clustered
SS.D1=5;                 % size D_1 of the self similarity hypercubes
SS.D2=5;                 % size D_2 of the self similarity hypercubes
% SS.D1=size(I,1);                 % size D_1 of the self similarity hypercubes
% SS.D2=size(I,2);                 % size D_2 of the self similarity hypercubes

% get an image specific dictionary
SS=SS.getClustersFromPixels({I});

% now compute the prototype assignment map and visualize it
M=SS.quantise(I);
imagesc(M);

% get the full image self similarity hypercube
% compute one "normalized" SSH
SSH=SS.getOneSSH(M);
td_visualizeSSH(SSH)

%  get 20 random boxes on this image (only for demo purposes)
% xmin=randi(ceil(size(M,2)/2),1,20); xmax=size(M,2)/2+randi(floor(size(M,2)/2),1,20);
% ymin=randi(ceil(size(M,1)/2),1,20); ymax=size(M,1)/2+randi(floor(size(M,1)/2),1,20);

% xmin=[1 xmin]; xmax=[size(M,2) xmax];
% ymin=[1 ymin]; ymax=[size(M,1) ymax];

[xmin, ymin] = meshgrid(1:W-5, 1:H-5);
xmin = reshape(xmin', 1, []);
ymin = reshape(ymin', 1, []);
[xmax, ymax] = meshgrid(6:W, 6:H);
xmax = reshape(xmax', 1, []);
ymax = reshape(ymax', 1, []);

% compute many "non-normalized" SSHs
SSHs=SS.getManySSH(M,xmin,ymin,xmax,ymax);
