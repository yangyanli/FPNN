#!/usr/bin/python
# -*- coding: utf-8 -*-

import math
import shutil
import tempfile
import argparse
from google.protobuf import text_format

#https://github.com/BVLC/caffe/issues/861#issuecomment-70124809
import matplotlib 
matplotlib.use('Agg')   

from utility_caffe import *

parser = argparse.ArgumentParser(description="Compute accuracy.")
parser.add_argument('-p', '--prototxt', help='Path to prototxt file', required=True)
parser.add_argument('-m', '--caffemodel', help='Path to caffe model', required=True)
parser.add_argument('-c', '--caffe_path', help='Path to caffe installation', required=True)
parser.add_argument('-f', '--feature', help='Name of the output feature', required=True)
parser.add_argument('-l', '--label', help='Name of the ground truth label', required=True)
parser.add_argument('-g', '--gpu_index', help='GPU to use', type=int, required=True)
args = parser.parse_args()

tempdir = tempfile.mkdtemp()
print 'Output LMDBs will be saved to', tempdir

features = [args.feature, args.label]
output_lmdbs = [os.path.join(tempdir, args.feature), os.path.join(tempdir, args.label)]
extract_features(prototxt=args.prototxt, caffemodel=args.caffemodel,
    features=features, output_lmdbs=output_lmdbs,
    caffe_path=args.caffe_path, gpu_index=args.gpu_index)
    
features = lmdb_to_array(output_lmdbs[0])
print features.shape
labels = lmdb_to_array(output_lmdbs[1]).flatten().tolist()

shutil.rmtree(tempdir)

if args.caffe_path:
    sys.path.append(os.path.join(args.caffe_path, 'python'))
import caffe
net_parameter = caffe.proto.caffe_pb2.NetParameter()
text_format.Merge(open(args.prototxt, 'r').read(), net_parameter)
if not net_parameter.layer:
    sample_num = get_lmdb_size(net_parameter.layers[0].data_param.source)
    transformation_num = net_parameter.layers[1].transform_3d_param.num_transformations
else:
    sample_num = get_lmdb_size(net_parameter.layer[0].data_param.source)
    transformation_num = net_parameter.layer[1].transform_3d_param.num_transformations

labels_predicted = []
for i in range(sample_num*transformation_num):
    feature = features[i, :].flatten().tolist()
    feature_exp = [math.exp(value) for value in feature]
    feature_exp_sum = sum(feature_exp)
    softmax = [value/feature_exp_sum for value in feature_exp]
    labels_predicted.append(softmax.index(max(softmax)))
    
correctness = [(x==y) for (x,y) in zip(labels_predicted, labels)]
print "Accuracy (%d x %d): "%(sample_num, transformation_num), sum(correctness)*1.0/len(correctness)