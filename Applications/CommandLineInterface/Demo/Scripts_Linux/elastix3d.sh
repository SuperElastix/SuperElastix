# data from Namic - Deformable registration speed optimization http://www.insight-journal.org/midas/collection/view/29
../SuperElastix --conf ../Configuration/elastix3d.json --loglevel debug --in FixedImage=N012_S03_tof3d_multi_slab.mha MovingImage=N026_S02_tof3d_multi_slab.mha --out ResultImage=elastix3d_warped.mhd ResultDisplacementField=elastix3d_displacement.mhd --log-level trace
