..\SuperElastix.exe --conf ..\Configuration\itkv4_SVF_MSD_missingController.json --graphout 2B_graph_itkv4_MSD_missingController.dot --in FixedImage=..\Data\coneA2d64.mhd MovingImage=..\Data\coneB2d64.mhd --out ResultImage=2B_image_itkv4_MSD_missingController.mhd ResultDisplacementField=2B_deformation_itkv4_MSD_missingController.mhd

CALL graphviz_to_png 2B_graph_itkv4_MSD.dot