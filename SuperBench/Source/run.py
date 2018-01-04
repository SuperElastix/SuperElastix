import argparse
import os
import register
import evaluate

parser = argparse.ArgumentParser(description='SuperBench registration driver.')

parser.add_argument('--registration-driver', '-rd', required=True, help="Registration driver")
parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
parser.add_argument('--output-directory', '-od', required=True)
parser.add_argument('--popi-input-directory', '-pid')

parameters = parser.parse_args()

if not os.path.isfile(parameters.registration_driver):
    raise Exception('Could not find registration driver ' + parameters.registration_driver + ".")

if not os.path.exists(parameters.submissions_directory):
    raise Exception('Could not find submission directory ' + parameters.submissions_directory + ".")

if not os.path.exists(parameters.output_directory):
    os.mkdir(parameters.output_directory)

register.run(parameters)
evaluate.run(parameters)
