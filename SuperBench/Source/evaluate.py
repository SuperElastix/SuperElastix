import logging, os, json
from register import load_submissions, load_datasets, get_category

def write_json(output_file_name, results):
    output_directory = os.path.dirname(output_file_name)
    if not os.path.exists(output_directory):
        os.mkdir(output_directory)

    with open(output_file_name, "w") as f:
        f.write(json.dumps(results, indent=4))

def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    for team_name, blueprint_file_name in submissions:
        blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
        category = get_category(blueprint_file_name)
        results = { team_name: { blueprint_name: dict() } }
        for dataset in datasets[category]:
            results[team_name][blueprint_name][dataset.name] = dict()
            for image_file_names, ground_truth_file_names, deformation_field_file_names in dataset.generator():
                output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)
                logging.info('Evalualting %s and %s.', image_file_names[0], image_file_names[1])
                try:
                    result_0, result_1 = dataset.evaluate(
                        parameters.registration_driver,
                        [os.path.join(output_directory, image_file_name) for image_file_name in image_file_names],
                        [os.path.join(output_directory, ground_truth_file_name) for ground_truth_file_name in ground_truth_file_names],
                        [os.path.join(output_directory, deformation_field_file_name) for deformation_field_file_name in deformation_field_file_names])

                    results[team_name][blueprint_name][dataset.name][deformation_field_file_names[0]] = result_0
                    results[team_name][blueprint_name][dataset.name][deformation_field_file_names[1]] = result_1
                except Exception as e:
                    results[team_name][blueprint_name][dataset.name][deformation_field_file_names[0]] = 'Error during evaluation'
                    results[team_name][blueprint_name][dataset.name][deformation_field_file_names[1]] = 'Error during evaluation'
                    logging.error('Error during evaulation of %s, %s, %s' % (team_name, blueprint_name, dataset.name))

        write_json(os.path.join(parameters.output_directory, "results.json"), results)
