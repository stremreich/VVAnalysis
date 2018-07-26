#!/usr/bin/env python
import configparser
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("variable", type=str)
args = parser.parse_args()

config = configparser.ConfigParser()
config.read_file(open("Templates/config"))
if not args.variable in config['Setup']:
    raise ValueError("variable %s does not exist in config file Template/config" % args.variable)

print config['Setup'][args.variable]

