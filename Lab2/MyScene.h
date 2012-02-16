#ifndef OSU_H
#define OSU_H
#include "OSUInventor.h"
#endif
#ifdef LINUX
#include <iostream>
using namespace std;
#else
#include <ostream.h>
#endif
// OpenInventor includes
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoLists.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoNormalElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoTextureImageElement.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoVertexShape.h>
#include <Inventor/nodes/SoNonIndexedShape.h>


class MyScene {
public:
        OSUInventorScene *scene;
        SbMatrix *transform_list;
        MyScene(OSUInventorScene *input_scene);
        SbMatrix* set_object();
};
