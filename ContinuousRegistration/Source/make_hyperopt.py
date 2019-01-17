import sys, os, time, glob, copy, shutil, numpy as np
from subprocess import Popen

from ContinuousRegistration.Source.make_registration_scripts import parser, run as make_registration_scripts
from ContinuousRegistration.Source.make_evaluation import run as make_evaluation
from ContinuousRegistration.Source.util import write_json
from ContinuousRegistration.Source.datasets import load_datasets

from hyperopt import fmin, tpe, hp


def exec_commands(parameters, cmds):
    ''' Exec commands in parallel in multiple process
    (as much as we have CPU)
    '''
    if not cmds: return # empty list

    def done(p):
        return p.poll() is not None
    def success(p):
        return p.returncode == 0
    def fail():
        sys.exit(1)

    processes = []
    while True:
        while cmds and len(processes) < parameters.jobs:
            task = cmds.pop()
            print('Running %s ...' % task)
            processes.append(Popen(task))

        for p in processes:
            if done(p):
                if success(p):
                    processes.remove(p)
                else:
                    fail()

        if not processes and not cmds:
            break
        else:
            time.sleep(1.0)

class objective():
    def __init__(self, parameters):
        self.parameters = parameters

        # Preprocess data
        self.datasets = load_datasets(self.parameters)

    def __call__(self, hyperopts):
        # Write new blueprint to disk
        blueprint = copy.deepcopy(parameters.blueprint)
        blueprint["Components"] += list(hyperopts.values())

        write_json(os.path.join(parameters.submissions_directory,
                                parameters.team_name,
                                self.parameters.blueprint_file_name[0]), blueprint)

        # Make shell scripts for running registrations. Scripts from previous iterations will be overridden
        make_registration_scripts(self.parameters)

        # Run registrations
        try:
            if sys.platform == 'win32':
                exec_commands(parameters,
                              glob.glob(os.path.join(parameters.output_directory, '*', '*', 'bat', '*.bat')))
            else:
                exec_commands(parameters,
                              glob.glob(os.path.join(parameters.output_directory, '*', '*', 'sh', '*.sh')))
        except Exception as e:
            print(e)

        # Evaluate and report target metric
        results = make_evaluation(parameters)
        result = []
        for team_name, team_results in results.items():
            for blueprint_name, blueprint_results in team_results.items():
                for dataset_name, dataset_results in blueprint_results.items():
                    for dataset_result in dataset_results:
                        for registration_name, registration_result in dataset_result.items():
                            result.append(registration_result[parameters.metric])

        return np.nanmean(results)

    def __del__(self):
        for file in os.listdir(self.parameters.output_directory):
            file_path = os.path.join(self.parameters.output_directory, file)
            try:
                if os.path.isfile(file_path):
                    os.remove(file_path)
                elif os.path.isdir(file_path):
                    shutil.rmtree(file_path)
            except Exception as e:
                print(e)


def run(parameters, space):
    fn = objective(parameters)
    best = fmin(fn=fn,
                space=space,
                algo=tpe.suggest,
                max_evals=parameters.maximum_number_of_blueprint_evaluations)

    print(best)

if __name__ == '__main__':
    parser.add_argument('--metric', '-m', type=str, required=True)
    parser.add_argument('--jobs', '-j', type=int, default=1)
    parser.add_argument('--maximum-number-of-blueprint-evaluations', '-mbe', type=int, default=64)
    parameters = parser.parse_args()

    parameters.blueprint = {
        "Datasets": ["POPI"],
        "Components": [
            {
                "Name": "FixedImage",
                "NameOfClass": "ItkToNiftiImageSourceComponent",
                "PixelType": "float",
                "Dimensionality": "3"
            },
            {
                "Name": "FixedMask",
                "NameOfClass": "ItkImageSourceComponent",
                "PixelType": "unsigned char",
                "Dimensionality": "3"
            },
            {
                "Name": "MovingImage",
                "NameOfClass": "ItkToNiftiImageSourceComponent",
                "PixelType": "float",
                "Dimensionality": "3"
            },
            {
                "Name": "MovingMask",
                "NameOfClass": "ItkImageSourceComponent",
                "PixelType": "unsigned char",
                "Dimensionality": "3"
            },
            {
                "Name": "SplineToDisplacementField",
                "NameOfClass": "NiftyregSplineToDisplacementFieldComponent"
            },
            {
                "Name": "DisplacementField",
                "NameOfClass": "DisplacementFieldNiftiToItkImageSinkComponent",
                "Dimensionality": "3"
            }
        ],
        "Connections": [
            {
                "Out": "FixedImage",
                "In": "NiftyRegAffine",
                "NameOfInterface": "NiftyregReferenceImageInterface"
            },
            {
                "Out": "MovingImage",
                "In": "NiftyRegAffine",
                "NameOfInterface": "NiftyregFloatingImageInterface"
            },
            {
                "Out": "NiftyRegAffine",
                "In": "NiftyRegSpline",
                "NameOfInterface": "NiftyregAffineMatrixInterface"
            },
            {
                "Out": "FixedImage",
                "In": "NiftyRegSpline",
                "NameOfInterface": "NiftyregReferenceImageInterface"
            },
            {
                "Out": "MovingImage",
                "In": "NiftyRegSpline",
                "NameOfInterface": "NiftyregFloatingImageInterface"
            },
            {
                "Out": "NiftyRegSpline",
                "In": "SplineToDisplacementField",
                "NameOfInterface": "NiftyregControlPointPositionImageInterface"
            },
            {
                "Out": "FixedImage",
                "In": "SplineToDisplacementField",
                "NameOfInterface": "NiftyregReferenceImageInterface"
            },
            {
                "Out": "SplineToDisplacementField",
                "In": "DisplacementField",
                "NameOfInterface": "NiftyregDisplacementFieldImageInterface"
            },
            {
                "Out": "FixedImage",
                "In": "DisplacementField",
                "NameOfInterface": "itkImageDomainFixedInterface"
            }
        ]
    }

    space = hp.choice('HyperOpt', [
        {
            "NiftyRegAffineHyperOptComponent": {
                "Name": "NiftyRegAffine",
                "NameOfClass": "NiftyregAladinComponent",
                "NumberOfResolutions": hp.quniform('NiftyRegAffine_NumberOfResolutions', 2, 5, 1),
                "NumberOfIterations": "30"
            },
            "NiftyRegBSplineHyperOptComponent": {
                "Name": "NiftyRegSpline",
                "NameOfClass": "Niftyregf3dComponent",
                "SmoothingSigma": hp.quniform('NiftyRegSpline_SmoothingSigma', 1, 8, 1),
                "NumberOfResolutions": hp.quniform('NiftyRegSpline_NumberOfResolutions', 2, 5, 1),
                "NumberOfIterations": "30"
            }
        }
    ])

    run(parameters, space)


