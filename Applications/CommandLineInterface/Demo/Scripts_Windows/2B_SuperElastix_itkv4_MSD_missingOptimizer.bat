..\SuperElastix.exe --conf ..\Configuration\itkv4_SVF_MSD_missingOptimizer.json --graphout 2B_graph_itkv4_MSD_missingOptimizer.dot --in FixedImage=..\Data\coneA2d64.mhd MovingImage=..\Data\coneB2d64.mhd --out ResultImage=2B_image_itkv4_MSD_missingOptimizer.mhd ResultDisplacementField=2B_deformation_itkv4_MSD_missingOptimizer.mhd

CALL graphviz_to_png 2B_graph_itkv4_MSD.dot