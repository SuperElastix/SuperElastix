import os, json, datetime
from ContinuousRegistration.Source.make_registration_scripts import parser
from ContinuousRegistration.Source.util import logging, load_submissions
from ContinuousRegistration.Source.datasets import load_datasets

def write_json(output_file_name, results):
    output_directory = os.path.dirname(output_file_name)
    if not os.path.exists(output_directory):
        os.mkdir(output_directory)

    with open(output_file_name, "w") as f:
        f.write(json.dumps(results, indent=4))

def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    results = {}
    for team_name, blueprint_file_names in submissions.items():
        for blueprint_file_name in blueprint_file_names:
            if hasattr(parameters, 'blueprint_file_name') and not parameters.blueprint_file_name is None:
                # User requested evaluation for this blueprint only
                if not parameters.blueprint_file_name == os.path.basename(blueprint_file_name):
                    continue

            if not team_name in results:
                results[team_name] = {}

            blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
            if not blueprint_name in results[team_name]:
                results[team_name][blueprint_name] = {}

            blueprint = json.load(open(blueprint_file_name))
            for dataset_name in blueprint['Datasets']:
                if not dataset_name in datasets:
                    logging.error('Dataset "%s" requested by blueprint "%s" '
                                  'but no data directory provided. Skipping dataset.',
                                  dataset_name, blueprint_file_name)
                    continue

                dataset = datasets[dataset_name]
                results[team_name][blueprint_name][dataset_name] = []

                logging.info('Evaluating registration scripts for %s blueprint and %s dataset.' % (
                blueprint_name, dataset_name))
                for file_names in dataset.generator():
                    output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)

                    logging.info('Evaluating image pair %s.', file_names['image_file_names'])
                    try:
                        results[team_name][blueprint_name][dataset.name].append(dataset.evaluate(
                            parameters.superelastix, file_names, output_directory))

                        if parameters.warp_images:
                            dataset.warp_images(parameters.superelastix, file_names, output_directory)

                        if parameters.warp_checkerboards:
                            dataset.warp_checkerboards(parameters.superelastix, file_names, output_directory)

                        if parameters.warp_image_checkerboards:
                            dataset.warp_image_checkerboards(parameters.superelastix, file_names, output_directory)

                        if parameters.warp_label_checkerboards:
                            dataset.warp_label_checkerboards(parameters.superelastix, file_names, output_directory)
                    except Exception as e:
                        logging.error('Error during evaluation of %s\'s blueprint %s and dataset %s: %s'
                                      % (team_name, blueprint_name, dataset.name, str(e)))


    write_json(os.path.join(parameters.output_directory,
                            'results-{:%Y-%m-%d-%H-%M-%S-%f}'.format(datetime.datetime.now()) + '.json'), results)

if __name__ == '__main__':
    parser.add_argument('--warp-images', '-wi', type=bool, default=False)
    parser.add_argument('--warp-checkerboards', '-wc', type=bool, default=False)
    parser.add_argument('--warp-image-checkerboards', '-wic', type=bool, default=False)
    parser.add_argument('--warp-label-checkerboards', '-wlc', type=bool, default=False)
    run(parser.parse_args())
