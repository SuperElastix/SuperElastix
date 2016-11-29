..\SuperElastix.exe --conf ..\Configuration\elastix_Bspline_NC.json --graphout 1A_graph_elastix_NC.dot --in FixedImage=..\Data\coneA2d64.mhd MovingImage=..\Data\coneB2d64.mhd --out ResultImage=1A_image_elastix_NC.mhd

CALL graphviz_to_png 1A_graph_elastix_NC.dot

