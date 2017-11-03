..\SuperElastix.exe --conf ..\Configuration\itkv4.json ..\Configuration\itkv4_Affine_MSD.json ..\Configuration\itk_TransformSink.json --graphout itkv4_Composed_Affine_MSD_TranformSink.dot --in FixedImage=..\Data\coneA2d64.mhd MovingImage=..\Data\coneB2d64.mhd --out ResultImage=itkv4_Composed_Affine_MSD_TranformSink.mhd ResultDisplacementField= itkv4_Composed_Affine_MSD_TranformSink_def.mhd ResultTransform=itkv4_Composed_Affine_MSD_TranformSink.tfm

CALL graphviz_to_png itkv4_Composed_Affine_MSD_TranformSink.dot
