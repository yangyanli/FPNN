#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import lmdb
import datetime
import numpy as np
from subprocess import call
from google.protobuf import text_format

#https://github.com/BVLC/caffe/issues/861#issuecomment-70124809
import matplotlib 
matplotlib.use('Agg')   

def extract_features(prototxt, caffemodel, features, output_lmdbs, caffe_path=None, gpu_index=0):
    if caffe_path:
        sys.path.append(os.path.join(caffe_path, 'python'))
    import caffe
    net_parameter = caffe.proto.caffe_pb2.NetParameter()
    text_format.Merge(open(prototxt, 'r').read(), net_parameter)
    if not net_parameter.layer:
        batch_size = net_parameter.layers[0].data_param.batch_size
        sample_num = get_lmdb_size(net_parameter.layers[0].data_param.source)
        transformation_num = net_parameter.layers[1].transform_3d_param.num_transformations
    else:
        batch_size = net_parameter.layer[0].data_param.batch_size
        sample_num = get_lmdb_size(net_parameter.layer[0].data_param.source)
        transformation_num = net_parameter.layer[1].transform_3d_param.num_transformations
   
    num_mini_batches = (sample_num+batch_size-1)/batch_size
    print 'Batch size is %d, and requires %d mini batches to process %d samples!'%(batch_size, num_mini_batches, sample_num)
        
    # extract features
    executable_path = os.path.join(caffe_path, 'bin', 'extract_features')
    args = [executable_path, caffemodel, prototxt, ','.join(features), ','.join(output_lmdbs), str(num_mini_batches), 'lmdb', 'GPU', str(gpu_index)]
    print args
    call(args)
    
    # crop lmdbs to length of sample_num
    for output_lmdb in output_lmdbs:
        lmdb_size = get_lmdb_size(output_lmdb)
        if lmdb_size == batch_size*num_mini_batches*transformation_num:
            num_cropping = (batch_size*num_mini_batches-sample_num)*transformation_num
        else:
            num_cropping = batch_size*num_mini_batches-sample_num
        if num_cropping != 0:
            print datetime.datetime.now().time(), "Cropping LMDBS %s..."%(output_lmdb)
            env = lmdb.open(output_lmdb, map_size=int(1e12))
            with env.begin(write=True) as txn:
                cursor = txn.cursor()
                for i in range(num_cropping):
                    cursor.last()
                    cursor.delete()
            env.close()
    
def get_lmdb_size(lmdb_folder):
    env = lmdb.open(lmdb_folder, readonly=True)
    with env.begin() as txn:
        lmdb_size = sum(1 for _ in txn.cursor())
    env.close()
    return lmdb_size

def lmdb_to_txt(lmdb_folder, caffe_path=None):
    if caffe_path:
        sys.path.append(os.path.join(caffe_path, 'python'))
    import caffe
 
    txt_filename = lmdb_folder+'.txt'
    print 'Saving', txt_filename, '...'
    with open(txt_filename, 'w') as txt_file:
        datum = caffe.proto.caffe_pb2.Datum()
        env = lmdb.open(lmdb_folder, readonly=True)
        with env.begin() as txn:
            cursor = txn.cursor()
            for _, value in cursor:
               datum.ParseFromString(value) 
               datum_np_array = caffe.io.datum_to_array(datum)
               feature = datum_np_array.flatten().tolist()
               txt_file.write(' '.join([str(f) for f in feature])+'\n')
        env.close()
        
def lmdb_to_array(lmdb_folder, caffe_path=None):
    if caffe_path:
        sys.path.append(os.path.join(caffe_path, 'python'))
    import caffe
 
    datum = caffe.proto.caffe_pb2.Datum()
    env = lmdb.open(lmdb_folder, readonly=True)
    array = np.empty([0])
    
    with env.begin() as txn:
        cursor = txn.cursor()
        sample_idx = 0
        for _, value in cursor:
           datum.ParseFromString(value) 
           datum_np_array = caffe.io.datum_to_array(datum)
           if array.size == 0:
               lmdb_size = get_lmdb_size(lmdb_folder)
               array = np.empty([lmdb_size]+list(datum_np_array.shape))
           array[sample_idx, :] = datum_np_array
           sample_idx = sample_idx + 1
    env.close()
    
    return array
