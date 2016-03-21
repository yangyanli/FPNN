#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import h5py
import lmdb
import datetime
import argparse
from multiprocessing import Pool

#https://github.com/BVLC/caffe/issues/861#issuecomment-70124809
import matplotlib 
matplotlib.use('Agg')

parser = argparse.ArgumentParser(description="Convert distance fields in .h5 format to lmdb.")
parser.add_argument('-i', '--filelist', help='Path to input mesh model filelist', required=True)
parser.add_argument('-d', '--df_list', help='Path to input distance field filelist', required=True)
parser.add_argument('-o', '--lmdb_folder', help='Ouput LMDB folder', required=True)
parser.add_argument('-c', '--caffe_path', help='Path to Caffe installation', required=True)
parser.add_argument('-p', '--pool_size', help='Pool size', type=int, default=12)
parser.add_argument('-f', '--save_float', help='Save data as float or not', action='store_true')
parser.add_argument('-t', '--txn_batch', help='LMDB transaction batch size', type=int, default=1024)
args = parser.parse_args()

if args.caffe_path:
  sys.path.append(os.path.join(args.caffe_path, 'python'))
from caffe.proto import caffe_pb2

def _datum_to_datum_string(datum):
  return datum.SerializeToString()

pool = Pool(args.pool_size)

env = lmdb.open(args.lmdb_folder, map_size=int(1e12))

sample_idx = 0
filenames = [line.strip().split()[-1] for line in open(args.df_list, 'r')]
labels = [int(line.strip().split()[-1]) for line in open(args.filelist, 'r')]
assert len(filenames) == len(labels)
datums = []
for idx, filename in enumerate(filenames):
  print datetime.datetime.now().time(), 'Converting data %d (%s) of %d...'%(idx, filename, len(filenames))
  hdf5_file = h5py.File(filename, 'r')
  data = hdf5_file['DenseField']
  label = labels[idx]
  print filename, label
  array_float = data[:, :, :]
  datum = caffe_pb2.Datum()
  datum.channels, datum.height, datum.width = array_float.shape
  if args.save_float:
    datum.float_data.extend(array_float.astype(dtype='float').flat) 
  else:
    array_uint8 = array_float.astype(dtype='uint8')
    datum.data = array_uint8.tostring()
  datum.label = label
  datums.append(datum)
    
  if (len(datums)%args.txn_batch == 0 or idx == len(filenames)-1) and len(datums) != 0:
    datum_strings = pool.map(_datum_to_datum_string, datums)
    with env.begin(write=True) as txn:
      for datum_string in datum_strings:
        txn.put('{:0>10d}'.format(sample_idx), datum_string)
        sample_idx = sample_idx + 1
    datums = []

env.close();
