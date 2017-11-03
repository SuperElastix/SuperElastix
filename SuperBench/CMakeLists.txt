# Testing variables
SUPERBENCH_DATASET_DIRLAB = ""
SUPERBENCH_DATASET_SPREAD = ""

for( SUPERBENCH_DATASET IN )

BlueprintPointer blueprint = Blueprint::New();
SuperElastixFilter::Pointer = SuperElastixFilter::New();
SuperElastixFilter->SetInput( image );
SuperElastixFilter->SetLogger( logger );
SuperElastixFilter->Set( blueprint );
SuperElastixFilter->Update();

logger->AddOutLogger();

SuperElastixFilter->Update();
