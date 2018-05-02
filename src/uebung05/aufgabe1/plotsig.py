# -*- coding: utf-8 -*-
"""
Created on Mon Oct 30 21:20:57 2017

@author: Ivo
"""

import numpy as np

import sys

A=sys.argv

if len(sys.argv) <= 1:
    fname = 'D:\\Home\\ivo\\Vorlesungen\\TEI2\\Allgemein\\Übungen\\audacityexperimente\\experimente\\experimente\\experimente\\fftampl.data'
else:
    fname = sys.argv[1]

data = np.loadtxt(fname, delimiter=',')

import matplotlib.pyplot as plt; plt.rcdefaults()
import numpy as np
import matplotlib.pyplot as plt

#data[:,2]=0

xlabels=('n','','Hz')
if np.max(data[:,2]) > 0:
    lid=2
else:
    lid=0

N=data.shape[0]
if len(sys.argv) > 2 and int(sys.argv[2]) > 0:
    N = min(N, int(sys.argv[2]))
    
#N=32
pos = np.arange(N)
if N<=32:    
    
    plt.bar(data[pos,0], data[pos,1], align='center', alpha=0.5)
    plt.xticks(data[pos,0], np.rint(data[pos,lid]).astype(int), rotation=40)
    #plt.ylabel('X[k]')
    #plt.title('FT coefficients')
else:
    plt.plot(data[pos,lid],data[pos,1])

plt.xlabel(xlabels[lid])

plt.show()
#==============================================================================
#import matplotlib as mpl
# mpl.rcParams['legend.fontsize'] = 10
# 
# fig = plt.figure()
# ax = fig.gca(projection='3d')
# ax.plot(data[:,3], data[:,4], data[:,5], label='parametric curve')
# 
# plt.show()
#==============================================================================
