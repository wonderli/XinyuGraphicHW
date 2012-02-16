#include "MyScene.h"
MyScene::MyScene(OSUInventorScene *input_scene) {
	this->scene = input_scene;
	int length = this->scene->Objects.getLength();
	this->transform_list = new SbMatrix[length];	
}

SbMatrix* MyScene::set_object() {
	OSUObjectData *object = NULL;
        SoTransform *transformation = NULL;

        SbMatrix T, S, R;

        SbVec3f scale_vector;
        SbRotation rotation;
        SbVec3f rotation_axis;
        float rotation_angle;
        SbVec3f translation_vector;

        int length = this->scene->Objects.getLength();
	int i = 0;

        for (i = 0; i < length; i++) {
                //OSUObjectData *object = (OSUObjectData *)scene->Objects[i];
                object = (OSUObjectData *)this->scene->Objects[i];
                if(!object->Check()) {
                        cerr << "Error in OSUObjectData for object" << i << endl;
                        exit(0);
                }

                transformation = object->transformation;
                translation_vector = transformation->translation.getValue();
                scale_vector = transformation->scaleFactor.getValue();
                rotation = transformation->rotation.getValue();
                rotation.getValue(rotation_axis, rotation_angle);

                T.makeIdentity();
                S.makeIdentity();
                R.makeIdentity();

                T.setTranslate(translation_vector);
                S.setScale(scale_vector);
                R.setRotate(rotation);

                this->transform_list[i].makeIdentity();
                this->transform_list[i].multRight(S);
                this->transform_list[i].multRight(R);
                this->transform_list[i].multRight(T);
        }
        return this->transform_list;

}


