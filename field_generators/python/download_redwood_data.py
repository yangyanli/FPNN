#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import datetime
import argparse
from subprocess import call

parser = argparse.ArgumentParser(description="Download redwood dataset.")
parser.add_argument('-u', '--urls_folder', help='Path to folder with URL files', required=True)
parser.add_argument('-d', '--download_folder', help='Path to download folder', required=True)
args = parser.parse_args()

url_files = [filename for filename in os.listdir(args.urls_folder) if os.path.isfile(os.path.join(args.urls_folder, filename)) and filename.endswith('.txt')]

for url_filename in url_files:
  category = filename[7:-4]
  category_folder = os.path.join(args.download_folder, category)
  if not os.path.exists(category_folder):
    os.makedirs(category_folder)
  urls = [line.strip() for line in open(os.path.join(args.urls_folder, url_filename), 'r')]
  for idx, url in enumerate(urls):
    print datetime.datetime.now().time(), 'Downloading %d of %d in category %s...'%(idx, len(urls), category)
    mesh_filename = os.path.join(args.download_folder, category, url.split('/')[-1])
    call(['wget', '-O', mesh_filename, url])