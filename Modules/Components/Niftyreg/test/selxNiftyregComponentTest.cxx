#include "_reg_f3d.h"

int component (int argc, char **argv)
{
	nifti_image *referenceImage=NULL;
	nifti_image *floatingImage=NULL;
	reg_f3d<float>* REG=new reg_f3d<float>(referenceImage->nt,floatingImage->nt);
}
