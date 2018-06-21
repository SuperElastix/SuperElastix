#!/bin/bash
../SuperElastix --conf ../Configuration/IdentityTransformRegistration.json --in FixedImage=../Data/coneA2d64.mhd MovingImage=../Data/coneB2d64.mhd --out DisplacementField=IdentityDeformationField.mhd --log-level trace
