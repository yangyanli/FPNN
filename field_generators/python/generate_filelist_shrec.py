#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import random
import argparse

parser = argparse.ArgumentParser(description="Generate filelist for SHREC dataset.")
parser.add_argument('-r', '--root_folder', help='Path to input root folder', required=True)
parser.add_argument('-o', '--output_folder', help='Path to output folder', required=True)
args = parser.parse_args()

if not os.path.exists(args.output_folder):
  os.makedirs(args.output_folder)

annotations_filename = os.path.join(args.root_folder, 'all.csv')
annotations = [line.strip().split(',') for line in open(annotations_filename, 'r')]
del annotations[0] # remove the header

categories = set()
sub_categories = set()
for annotation in annotations:
  categories.add(annotation[1])
  sub_categories.add(annotation[2])
category_list = sorted(list(categories))
sub_category_list = sorted(list(sub_categories))

with open(os.path.join(args.output_folder, 'category_list.txt'), 'w') as file_category_list:
  for item in category_list:
    file_category_list.write(item+'\n')
with open(os.path.join(args.output_folder, 'sub_category_list.txt'), 'w') as file_sub_category_list:
  for item in sub_category_list:
    file_sub_category_list.write(item+'\n')

category_dict = dict(zip(category_list, range(len(category_list))))
sub_category_dict = dict(zip(sub_category_list, range(len(sub_category_list))))

ignore_list = [line.strip() for line in open(os.path.join(args.root_folder, 'ignore.txt'))]

datasets = ['train', 'val', 'test']
for dataset in datasets:
  filelist = []
  dataset_folder = dataset
  if not os.path.exists(os.path.join(args.root_folder, dataset)):
    dataset_folder = dataset+'perturbed'
  for annotation in annotations:
    if annotation[-1] == dataset and annotation[0] not in ignore_list:
      filename = os.path.join(dataset, annotation[1], 'model_'+annotation[0]+'.obj')
      if not os.path.exists(os.path.join(args.root_folder, filename)):
        filename = os.path.join(dataset, 'model_'+annotation[0]+'.obj')
      if os.path.exists(os.path.join(args.root_folder, filename)):
        filelist.append((filename, category_dict[annotation[1]], sub_category_dict[annotation[2]]))
  random.seed(9527)
  random.shuffle(filelist)
  with open(os.path.join(args.output_folder, 'filelist_'+dataset+'.txt'), 'w') as file_filelist:
    for item in filelist:
      file_filelist.write(item[0]+' '+str(item[1])+'\n')
  with open(os.path.join(args.output_folder, 'filelist_'+dataset+'_sub_category.txt'), 'w') as file_filelist:
    for item in filelist:
      file_filelist.write(item[0]+' '+str(item[2])+'\n')
