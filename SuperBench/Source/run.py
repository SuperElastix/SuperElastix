import argparse
import os
import register
import evaluate

parser = argparse.ArgumentParser(description='SuperBench registration driver.')

parser.add_argument('--registration-driver', '-rd', required=True, help="Registration driver")
parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
parser.add_argument('--output-directory', '-od', required=True)

parser.add_argument('--cumc12-input-directory', '-cid')
parser.add_argument('--dirlab-input-directory', '-did')
parser.add_argument('--empire-input-directory', '-eid')
parser.add_argument('--isbr18-input-directory', '-iid')
parser.add_argument('--lpba40-input-directory', '-lid')
parser.add_argument('--mgh10-input-directory', '-mid')
parser.add_argument('--spread-input-directory', '-sid')
parser.add_argument('--popi-input-directory', '-pid')

parameters = parser.parse_args()

if not os.path.isfile(parameters.registration_driver):
    raise Exception('Could not find registration driver ' + parameters.registration_driver + ".")

if not os.path.exists(parameters.submissions_directory):
    raise Exception('Could not find submission directory ' + parameters.submissions_directory + ".")

if not os.path.exists(parameters.output_directory):
    os.mkdir(parameters.output_directory)

# register.run(parameters)
evaluate.run(parameters)
