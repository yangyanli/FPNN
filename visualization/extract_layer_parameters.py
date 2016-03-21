#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import argparse

#https://github.com/BVLC/caffe/issues/861#issuecomment-70124809
import matplotlib 
matplotlib.use('Agg')

parser = argparse.ArgumentParser(description="Extract layer parameters for visualization.")
parser.add_argument('-i', '--caffemodel', help='Input caffemodel', required=True)
parser.add_argument('-c', '--caffe_path', help='Path to Caffe installation', required=True)
parser.add_argument('-l', '--layer_name', help='Layer to be extracted', required=True)
parser.add_argument('-d', '--diff', help='Extract diff, instead of data', action='store_true')
args = parser.parse_args()

if args.caffe_path:
  sys.path.append(os.path.join(args.caffe_path, 'python'))
from caffe.proto import caffe_pb2

with open(args.caffemodel, 'rb') as file_caffemodel:
    binary_caffemodel = file_caffemodel.read()
    net_parameter = caffe_pb2.NetParameter()
    net_parameter.ParseFromString(binary_caffemodel)
    layer_exists = False
    layer_weight_exists = False
    all_zeroes = True
    for layer in net_parameter.layer:
       if layer.name == args.layer_name:
         layer_exists = True
         for idx, blob in enumerate(layer.blobs):
           layer_weight_exists = True
           if args.diff:
             filename = args.caffemodel.replace('.caffemodel', '_layer_%s_blob_%d_diff.txt'%(layer.name, idx))
           else:
             filename = args.caffemodel.replace('.caffemodel', '_layer_%s_blob_%d.txt'%(layer.name, idx))
           print 'Saving layer parameters to %s...'%(filename)
           with open(filename, 'w') as file_output:
             blob_shape = []
             for dim in blob.shape.dim:
               blob_shape.append(str(dim))
             file_output.write(' '.join(blob_shape)+'\n')
             if args.diff:
               for value in blob.diff:
                 file_output.write(str(value) + '\n')
               for value in blob.diff:
                 if value != 0:
                   all_zeroes = False
             else:
               for value in blob.data:
                 file_output.write(str(value) + '\n')
               for value in blob.data:
                 if value != 0:
                   all_zeroes = False
    if not layer_exists:
      print 'Cannot find layer with name \"%s\" in the caffemodel.'%(args.layer_name)
    elif not layer_weight_exists:
      print 'Layer with name \"%s\" doesn\'t have any parameters.'%(args.layer_name)

    if all_zeroes:
      print 'Warning: the data contains all zeroes!!!'
