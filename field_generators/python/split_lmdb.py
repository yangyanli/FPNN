#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import lmdb
import datetime
import argparse
from multiprocessing import Pool

#https://github.com/BVLC/caffe/issues/861#issuecomment-70124809
import matplotlib 
matplotlib.use('Agg')

parser = argparse.ArgumentParser(description="Split lmdb by label index.")
parser.add_argument('-l', '--lmdb_folder', help='Path to input lmdb folder', required=True)
parser.add_argument('-i', '--label_index', help='The splitting label index', type=int, default=12)
parser.add_argument('-p', '--pool_size', help='Pool size', type=int, default=12)
parser.add_argument('-t', '--txn_batch', help='LMDB transaction batch size', type=int, default=1024)
parser.add_argument('-c', '--caffe_path', help='Path to Caffe installation', required=True)
args = parser.parse_args()

if args.caffe_path:
  sys.path.append(os.path.join(args.caffe_path, 'python'))
from caffe.proto import caffe_pb2

def _update_datum_string(datum_string):
  datum = caffe_pb2.Datum()
  datum.ParseFromString(datum_string)
  if datum.label < args.label_index:
    return (datum_string, 0)
  else:
    datum.label = datum.label-args.label_index
    return (datum.SerializeToString(), 1)

pool = Pool(args.pool_size)

env_part_0 = lmdb.open(args.lmdb_folder+'_part_0', map_size=int(1e12))
env_part_1 = lmdb.open(args.lmdb_folder+'_part_1', map_size=int(1e12))

env = lmdb.open(args.lmdb_folder, readonly=True)
with env.begin() as txn:
  lmdb_size = sum(1 for _ in txn.cursor())
datum = caffe_pb2.Datum()
with env.begin() as txn:
  cursor = txn.cursor()
  count = 0
  key_list = []
  datum_string_list = []
  for key, value in cursor:
    key_list.append(key)
    datum_string_list.append(value)
    count = count + 1
    
    if count%(2*args.txn_batch) == 0 or count == lmdb_size:
      datum_string_split_list = pool.map(_update_datum_string, datum_string_list)
      with env_part_0.begin(write=True) as txn_part_0:
        for idx, item in enumerate(datum_string_split_list):
          if item[1] == 0:
            txn_part_0.put(key_list[idx], item[0])
      with env_part_1.begin(write=True) as txn_part_1:
        for idx, item in enumerate(datum_string_split_list):
          if item[1] == 1:
            txn_part_1.put(key_list[idx], item[0])
      key_list = []
      datum_string_list = []
    
    if count%100 == 0:
      print datetime.datetime.now().time(), 'Splitted data %d of %d...'%(count, lmdb_size)
env.close()

env_part_0.close()
env_part_1.close()