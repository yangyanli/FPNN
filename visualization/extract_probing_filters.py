#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import json
import argparse

#https://github.com/BVLC/caffe/issues/861#issuecomment-70124809
import matplotlib 
matplotlib.use('Agg')

parser = argparse.ArgumentParser(description="Extract field probing filters for visualization.")
parser.add_argument('-i', '--caffemodel', help='Input caffemodel', required=True)
parser.add_argument('-c', '--caffe_path', help='Path to Caffe installation', required=True)
parser.add_argument('-r', '--resolution', help='Distance field resolution', type=int, required=True)
args = parser.parse_args()

if args.caffe_path:
  sys.path.append(os.path.join(args.caffe_path, 'python'))
from caffe.proto import caffe_pb2

field_probing_blob = None
curv_blob = None

with open(args.caffemodel, 'rb') as file_caffemodel:
    binary_caffemodel = file_caffemodel.read()
    net_parameter = caffe_pb2.NetParameter()
    net_parameter.ParseFromString(binary_caffemodel)
    for layer in net_parameter.layer:
       if layer.name == 'field_probing':
           field_probing_blob = layer.blobs[0]
           num_curve = layer.field_probing_param.num_curve
           len_curve = layer.field_probing_param.len_curve
       elif layer.name == 'curv':
           curv_blob = layer.blobs[0]

    if field_probing_blob == None:
        print 'Cannot find layer with name \"%s\" in the caffemodel.'%('field_probing')
    elif curv_blob == None:
        print 'Cannot find layer with name \"%s\" in the caffemodel.'%('curv')
    else:
        json_obj = dict()
        count = num_curve*len_curve
        dims = [num_curve, len_curve]
        json_obj['dims'] = dims
        num_channel = len(curv_blob.data)/count
        
        samples = []
        for idx in range(count):
            x = field_probing_blob.data[4*idx+0]/args.resolution
            y = field_probing_blob.data[4*idx+1]/args.resolution
            z = field_probing_blob.data[4*idx+2]/args.resolution
            data = curv_blob.data[idx*num_channel : idx*num_channel + num_channel]
            sample = [x, y, z] + data
            samples.append([round(value,8) for value in sample])
        json_obj['samples'] = samples
        
        filename = args.caffemodel.replace('.caffemodel', '_probing_filters.json')
        print 'Saving probing filters to %s...'%(filename)
        with open(filename, 'w') as file_json:
            json.dump(json_obj, file_json, indent=4, separators=(',', ': '))
        
