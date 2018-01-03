import argparse
import logging
import os
import subprocess
import inspect

import SimpleITK as sitk
import numpy as np

from register import load_submissions, load_datasets, get_category


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
            for image_file_names, ground_truth_file_names, deformation_field_file_names in dataset.generator():
                output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)
                logging.info('Evalualting %s and %s.', image_file_names[0], image_file_names[1])
                try:
                    results[dataset.name][deformation_field_file_names[0]], results[dataset.name][deformation_field_file_names[1]] = \
                        dataset.evaluate(parameters.registration_driver,
                                         [os.path.join(output_directory, image_file_name) for image_file_name in image_file_names],
                                         [os.path.join(output_directory, ground_truth_file_name) for ground_truth_file_name in ground_truth_file_names],
                                         [os.path.join(output_directory, deformation_field_file_name) for deformation_field_file_name in deformation_field_file_names])
                except Exception as e:
                    results[dataset.name][deformation_field_file_names[0]] = e
                    results[dataset.name][deformation_field_file_names[1]] = e
                    logging.error(e)

    write(results)

