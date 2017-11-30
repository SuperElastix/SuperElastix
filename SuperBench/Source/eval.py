import argparse
import logging
import os
import subprocess

import SimpleITK as sitk
import numpy as np

from .test import load_submissions, load_datasets, get_category
from .popi import POPI

def eval_singularity_ratio(deformation_field):
    deformation_field_array = sitk.GetArrayViewFromImage(deformation_field)
    return {
        'SingularityRatio': np.sum(deformation_field_array < 0)/np.prod(deformation_field_array.shape)
    }

def eval_point_sets(point_set_0, point_set_1):
    l2 = np.sqrt(np.sum((point_set_0-point_set_1)**2, 1))
    return {
        'TRE': np.mean(l2),
        'Hausdorff': np.max(l2)
    }

def eval_labels(labels):
    pass

def write(results):
    print(results)

def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_name in submissions:
        blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
        category = get_category(blueprint_file_name)
        results = dict()
        for dataset in datasets[category]:
            results[dataset.name] = dict()
            for image_file_name_pair, ground_truth_file_name_pair, relative_deformation_field_file_name_pair in dataset.generator():
                output_directory = os.path.join(parameters.output_directory,
                                                team_name,
                                                blueprint_name,
                                                os.path.dirname(relative_deformation_field_file_name_pair))
                output_file_name = os.path.join(output_directory,
                                                relative_deformation_field_file_name_pair)

                try:
                    logging.info('Evalualting %s and %s.', image_file_name_pair[0], image_file_name_pair[1])
                    results[dataset.name][relative_deformation_field_file_name_pair[0]],
                    results[dataset.name][relative_deformation_field_file_name_pair[1]] = dataset.evaluate(parameters.superelastix,
                                                                                                           image_file_name_pair,
                                                                                                           ground_truth_file_name_pair,
                                                                                                           output_file_name)
                except Exception as e:
                    results[dataset.name][relative_deformation_field_file_name_pair[0]] = e.output
                    results[dataset.name][relative_deformation_field_file_name_pair[1]] = e.output
                    logging.error(e.output)

    write(results)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='SuperBench registration driver.')

    parser.add_argument('--superelastix', '-selx', required=True, help="SuperElastix executable.")
    parser.add_argument('--submissions-directory', '-sd', required=True, help='Directory with parameter files.')
    parser.add_argument('--output-directory', '-od', required=True)
    parser.add_argument('--popi-input-directory', '-pid')

    parameters = parser.parse_args()
    run(parameters)

