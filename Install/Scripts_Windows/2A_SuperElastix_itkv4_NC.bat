..\SuperElastix.exe --conf ..\Configuration\itkv4_SVF_ANTsCC.json --graphout 2A_graph_itkv4_NC.dot --in FixedImage=..\Data\coneA2d64.mhd MovingImage=..\Data\coneB2d64.mhd --out ResultImage=1B_image_elastix_MSD.mhd ResultDisplacementField=2A_deformation_itkv4_NC.mhd

CALL graphviz_to_png 2A_graph_itkv4_NC.dot