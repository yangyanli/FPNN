#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import random
import argparse

parser = argparse.ArgumentParser(description="Generate filelist for ModelNet dataset.")
parser.add_argument('-r', '--root_folder', help='Path to input root folder', required=True)
parser.add_argument('-o', '--output_folder', help='Path to output folder', required=True)
args = parser.parse_args()

if not os.path.exists(args.output_folder):
  os.makedirs(args.output_folder)

category_list = sorted([category for category in os.listdir(args.root_folder) if os.path.isdir(os.path.join(args.root_folder, category))])
with open(os.path.join(args.output_folder, 'category_list.txt'), 'w') as file_category_list:
  for item in category_list:
    file_category_list.write(item+'\n')

filelist_train = []
filelist_test = []
for idx, category in enumerate(category_list):
  folder = os.path.join(args.root_folder, category)
  filelist = [(os.path.join(category, md5, 'model.obj'), idx) for md5 in os.listdir(folder)]
  random.seed(9527)
  random.shuffle(filelist)
  split_point = int(len(filelist)*0.8)
  filelist_train.extend(filelist[:split_point])
  filelist_test.extend(filelist[split_point:])

random.seed(9527)
random.shuffle(filelist_train)
with open(os.path.join(args.output_folder, 'filelist_train.txt'), 'w') as file_filelist:
  for item in filelist_train:
    file_filelist.write(item[0]+' '+str(item[1])+'\n')

random.seed(9527)
random.shuffle(filelist_test)
with open(os.path.join(args.output_folder, 'filelist_test.txt'), 'w') as file_filelist:
  for item in filelist_test:
    file_filelist.write(item[0]+' '+str(item[1])+'\n')
