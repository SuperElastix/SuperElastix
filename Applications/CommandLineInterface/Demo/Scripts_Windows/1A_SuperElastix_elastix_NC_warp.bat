..\SuperElastix.exe --conf ..\Configuration\elastix_Bspline_NC.json ..\Configuration\warp_3rd_image.json --graphout 1A_graph_elastix_NC_warp.dot --in FixedImage=..\Data\coneA2d64.mhd MovingImage=..\Data\coneB2d64.mhd WarpInputImage=..\Data\coneA2d64_label.mhd --out ResultImage=1A_image_elastix_NC.mhd WarpOutputImage=1A_image_elastix_NC_label.mhd

CALL graphviz_to_png 1A_graph_elastix_NC.dot

