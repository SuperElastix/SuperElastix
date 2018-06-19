# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:41:36 2016

@author: fberendsen
"""
import SimpleITK as sitk
import numpy as np

dimensions = 2
cubesize = 64

coordslice = slice(-(cubesize-1)/2.0, (cubesize+1)/2.0, None)

coordgrid = np.mgrid[tuple([coordslice]*dimensions)]

sphereR = cubesize*0.4

conefunc = lambda coord: np.sqrt(np.sum(coord**2))
pyramidfunc = lambda coord: np.max(np.abs(coord))

stretchfunc = lambda coord, stretch: stretch.dot(coord)

coneAfunc = lambda coord: 255.0 - 16.0 * conefunc(stretchfunc(coord,np.array([[0.9,0.5],[-0.1,0.9]])))
coneBfunc = lambda coord: 255.0 - 16.0 * pyramidfunc(stretchfunc(coord,np.array([[0.9,-0.5],[0.1,0.9]])))

coneA = np.apply_along_axis(coneAfunc,0,coordgrid)
coneB = np.apply_along_axis(coneBfunc,0,coordgrid)

sitk.WriteImage(sitk.GetImageFromArray(np.clip(coneA,0,255)),'coneA2d64.mhd')
sitk.WriteImage(sitk.GetImageFromArray(np.clip(coneB,64,255)),'coneB2d64.mhd')



coneA_shiftfunc = lambda coord: 255.0 - 16.0 * conefunc(stretchfunc(coord+[5,0],np.array([[0.9,0.5],[-0.1,0.9]])))
coneA_shift = np.apply_along_axis(coneA_shiftfunc,0,coordgrid)
sitk.WriteImage(sitk.GetImageFromArray(np.clip(coneA_shift,0,255)),'coneA2d64_shift.mhd')
