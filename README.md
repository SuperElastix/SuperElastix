# SuperElastix image registration toolbox #

[![Build Status](https://travis-ci.org/SuperElastix/SuperElastix.svg?branch=develop)](https://travis-ci.org/SuperElastix/SuperElastix)
[![codecov](https://codecov.io/gh/SuperElastix/SuperElastix/branch/develop/graph/badge.svg)](https://codecov.io/gh/SuperElastix/SuperElastix)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f391d3e6315745db938c532b708cf7e9)](https://www.codacy.com/app/SuperElastix/SuperElastix?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=SuperElastix/SuperElastix&amp;utm_campaign=Badge_Grade)

The objective of image registration is to find the spatial relationship between two or more images. 
In the last decades numerous image registration methods and tools have emerged from the research community. 
Implementation of these methods, however, are scattered over a plethora of toolboxes each with their own interface, limitations and modus operandi. 

SuperElastix is an image registration toolbox written in C++ that incorporates a diverse set of registration algorithms from third-party sources. 
Its modular and flexible architecture enables the integration of algorithms from any registration paradigm, while fully supporting cross-fertilization. 
It provides a unified user-interface by means of a configuration file. 
Commandline as well as library interfaces are readily available. 
SuperElastix leverages social coding practices from GitHub to make it a collaborative effort from the image registration community. 
It is developed in conjunction with the [Continuous Registration Challenge](https://continuousregistration.grand-challenge.org/), in which all the methods that are included in SuperElastix are automatically and continuously benchmarked on a diverse dataset.

## Documentation ##
- [Read-the-Docs](http://superelastix.readthedocs.io/en/latest/index.html)
- [Contributing](CONTRIBUTING.md)
- ["The design of SuperElastix - a unifying framework for a wide range of image registration methodologies"](http://www.cv-foundation.org/openaccess/content_cvpr_2016_workshops/w15/papers/Berendsen_The_Design_of_CVPR_2016_paper.pdf), F. F. Berendsen, K. Marstal, S. Klein and M. Staring, [WBIR2016](http://wbir2016.doc.ic.ac.uk/), [[paper latex source](https://github.com/SuperElastix/Publications/tree/master/paperWBIR16)] [[poster](https://github.com/SuperElastix/Publications/blob/master/posterWBIR16/Poster_SuperElastix_WBIR.svg)]

## Development ##
- `SuperElastix v1.0` has been released! Please see the `master` branch
- Users that want to build SuperElastix from sources are encouraged to use the `develop` branch
- All branches and pull requests are build and unit tested by Continuous Integration: see our [CDash](http://trunk.cdash.org/index.php?project=SuperElastix) for the latest status
