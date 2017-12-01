import argparse
import logging
import os
import subprocess

import SimpleITK as sitk
import numpy as np

from .test import load_submissions, load_datasets, get_category
from .popi import POPI

def eval_deformation_field(superelastix, point_set_file_name_pair, deformation_field_file_name_pair):
    # TODO: Implement inverse consistensy
    # For both point sets, warp points forward->backward, then call eval_point_sets and save TRE

    deformation_field_array_0 = sitk.GetArrayViewFromImage(sitk.ReadImage(deformation_field_file_name_pair[0]))
    deformation_field_array_1 = sitk.GetArrayViewFromImage(sitk.ReadImage(deformation_field_file_name_pair[1]))
    return ({
        'SingularityRatio': np.sum(deformation_field_array_0 < 0)/np.prod(deformation_field_array_0.shape),
    }, {
        'SingularityRatio': np.sum(deformation_field_array_1 < 0)/np.prod(deformation_field_array_1.shape),
    })

def eval_point_sets(point_set_file_name_pair):
    point_set_0 = np.loadtxt(point_set_file_name_pair[0])
    point_set_1 = np.loadtxt(point_set_file_name_pair[1])
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
            for image_file_name_pair, ground_truth_file_name_pair, deformation_field_relative_file_name_pair in dataset.generator():
                output_directory = os.path.join(parameters.output_directory,
                                                team_name,
                                                blueprint_name,
                                                os.path.dirname(deformation_field_relative_file_name_pair))
                output_file_name = os.path.join(output_directory,
                                                deformation_field_relative_file_name_pair)

                logging.info('Evalualting %s and %s.', image_file_name_pair[0], image_file_name_pair[1])
                try:
                    results[dataset.name][deformation_field_relative_file_name_pair[0]],
                    results[dataset.name][deformation_field_relative_file_name_pair[1]] = dataset.evaluate(parameters.superelastix,
                                                                                                           image_file_name_pair,
                                                                                                           ground_truth_file_name_pair,
                                                                                                           deformation_field_relative_file_name_pair)
                except Exception as e:
                    results[dataset.name][deformation_field_relative_file_name_pair[0]] = e.output
                    results[dataset.name][deformation_field_relative_file_name_pair[1]] = e.output
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

