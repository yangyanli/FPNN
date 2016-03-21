#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import argparse

parser = argparse.ArgumentParser(description="Generate distance field list.")
parser.add_argument('-r', '--root_folder', help='Path to input root folder', required=True)
parser.add_argument('-f', '--filelist', help='Path to input filelist', required=True)
parser.add_argument('-o', '--output_folder', help='Path to output folder', required=True)
parser.add_argument('-x', '--resolution', help='Distance field resolution', required=True)
args = parser.parse_args()

if not os.path.exists(args.output_folder):
  os.makedirs(args.output_folder)

filelist = [line.split(' ')[0] for line in open(args.filelist, 'r')]
filename_df_list = os.path.split(args.filelist)[-1].replace('filelist', 'df_'+str(args.resolution)+'_list')
with open(os.path.join(args.output_folder, filename_df_list), 'w') as file_df_list:
  for item in filelist:
    filename_source = os.path.join(args.root_folder, item)
    if '.off' in item:
      filename_target = os.path.join(args.output_folder, "hdf5_"+str(args.resolution), item.replace('.off', '.h5'))
    elif '.obj' in item:
      filename_target = os.path.join(args.output_folder, "hdf5_"+str(args.resolution), item.replace('.obj', '.h5'))
    file_df_list.write(filename_source + ' ' + str(args.resolution) + ' ' + filename_target + '\n')
