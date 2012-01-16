#ifdef LINUX
#include <iostream>
#include <vector>
using namespace std;
#else
#include <iostream.h>
#include <vector.h>
#endif

#include <math.h>

#include <Inventor/SbLinear.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoPointLight.h>

#include "OSUInventor.h"

void usage_error();
int main(int argc, char **argv) {
	if (argc != 2)
		usage_error();
	SoDB::init();
	OSUInventorScene *scene = new OSUInventorScene(argv[1]);
	/*Set up camera*/
	SoCamera *camera = NULL;



	return 0;


}
void usage_error() {
	cerr << "Usage: rt <input.iv> <output.ppm> <xres> <yres>" << endl;
	exit(10);
}
