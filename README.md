This repository contains all the code used for the Galaxy Formation Thesis.
The galaxy models (and some of the earlier models) are split into multiple parts.
The main code provides the actual simulation and creates output files on completion that are intended to be read and interpreted using seperate scripts.
The galaxy models are simulated using C++ to take advantage of its speed, and the ceated .txt files can be read using the various tools, depending on what the user intends to analyse.
These tools could be split into multiple parts themselves. For example, C++ is also used to measure the fractal dimension of the grid at each timestep, again to take advantage of its speed.
The output file of that script can then be interpreted using python as many of the relationships draw from the same correlation data file.
Details of which scripts to use to extract each relationship of figure is detailed within the scripts.
Example in MBSOGM_III.cpp, to measure fractal dimension, the user is lead to fractal_dimension_calculation.cpp and from there, lead to fractal_analysis.py.

Example figures extracted using this code is also provided for. However, if the user wishes to produce their own figures, the models must first be run to produce the necessary data files.
The Benchmarks folder contains scripts used for testing. However they are just adaptations of the code already present elsewhere, simplified to remove unnecessary code when testing.
The modified D-T SOGM is also presented in this folder if one wishes to see exactly what was changed (logically) from the original model formulateed by Mondero (2025) - which can be found in his thesis.

Thank you
TJ
