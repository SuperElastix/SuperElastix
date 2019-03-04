import os, json, datetime
from ContinuousRegistration.Source.make_registration_scripts import parser
from ContinuousRegistration.Source.util import logging, load_submissions, write_json
from ContinuousRegistration.Source.datasets import load_datasets

def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    results = {}
    for team_name, blueprint_file_names in submissions.items():
        for blueprint_file_name in blueprint_file_names:
            if not team_name in results:
                results[team_name] = {}

            blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
            if not blueprint_name in results[team_name]:
                results[team_name][blueprint_name] = {}

            logging.info('Loading blueprint %s/%s.' % (team_name, os.path.basename(blueprint_name)))
            blueprint = json.load(open(blueprint_file_name))
            for dataset_name in blueprint['Datasets']:
                if not dataset_name in datasets:
                    continue

                dataset = datasets[dataset_name]
                results[team_name][blueprint_name][dataset_name] = []

                for file_names in dataset.generator():
                    output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)

                    logging.info('Evaluating registration for blueprint %s and images %s.', blueprint_name, file_names['image_file_names'])
                    try:
                        results[team_name][blueprint_name][dataset.name].append(dataset.evaluate(
                            parameters.superelastix, file_names, output_directory))

                        if hasattr(parameters, 'make_images') and parameters.make_images:
                            dataset.make_images(parameters.superelastix, file_names, output_directory)

                        if hasattr(parameters, 'make_labels') and parameters.make_labels:
                            dataset.make_labels(parameters.superelastix, file_names, output_directory)

                        if hasattr(parameters, 'make_difference_images') and parameters.make_difference_images:
                            dataset.make_difference_images(parameters.superelastix, file_names, output_directory)

                        if hasattr(parameters, 'make_checkerboards') and parameters.make_checkerboards:
                            dataset.make_checkerboards(parameters.superelastix, file_names, output_directory)

                        if hasattr(parameters, 'make_image_checkerboards') and parameters.make_image_checkerboards:
                            dataset.make_image_checkerboards(parameters.superelastix, file_names, output_directory)

                        if hasattr(parameters, 'make_label_checkerboards') and parameters.make_label_checkerboards and dataset.name in ["CUMC12", "IBSR18", "LPBA40", "MGH10"]:
                            dataset.make_label_checkerboards(parameters.superelastix, file_names, output_directory)
                    except Exception as e:
                        logging.error('Error during evaluation of %s\'s blueprint %s and dataset %s: %s'
                                      % (team_name, blueprint_name, dataset.name, str(e)))

    write_json(os.path.join(parameters.output_directory,
                            'results.json'), results)

    return results


if __name__ == '__main__':
    parser.add_argument('--make-images', '-mi', type=bool, default=False, help="Warp moving images.")
    parser.add_argument('--make-labels', '-ml', type=bool, default=False, help="Warp moving labels.")
    parser.add_argument('--make-difference-images', '-mdi', type=bool, default=False, help="Warp moving images and subtract from fixed images.")
    parser.add_argument('--make-checkerboards', '-mc', type=bool, default=False, help="Warp checkerboard pattern.")
    parser.add_argument('--make-image-checkerboards', '-mic', type=bool, default=False,
                        help="Warp moving images and make checkerboard with fixed and warped moving image.")
    parser.add_argument('--make-label-checkerboards', '-mlc', type=bool, default=False,
                        help="Warp moving labels and make checkerboard with fixed and warped moving label.")
    run(parser.parse_args())
