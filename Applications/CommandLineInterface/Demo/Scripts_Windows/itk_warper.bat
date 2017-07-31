..\SuperElastix.exe --conf ..\Configuration\itk_warper.json --graphout itk_warper.dot --in FixedAndMovingImageSource=..\Data\coneA2d64.mhd TransformSource=..\Data\ItkAffine2Dtransform.tfm --out ResultImage=itk_warper.mhd

CALL graphviz_to_png itk_warper.dot
