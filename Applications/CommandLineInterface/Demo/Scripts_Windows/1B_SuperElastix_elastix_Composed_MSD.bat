..\SuperElastix.exe --conf ..\Configuration\elastix_Composed_with_Bspline_MSD.json --graphout 1B_graph_elastix_Composed_MSD.dot --in FixedImage=..\Data\coneA2d64.mhd MovingImage=..\Data\coneB2d64.mhd --out ResultImage=1B_image_elastix_Composed_MSD.mhd

CALL graphviz_to_png 1B_graph_elastix_MSD.dot
