#ifndef STUDIO_H
#define STUDIO_H

//	classes that contain data about studio files and what's in them

#include "thing.h"
#include "studio_item.h"
#include "studio_female.h"
#include "studio_male.h"
#include "transform.h"
#include "thing_group.h"
#include "midpoint.h"


class SceneFile {
	string pathname;
	string filename;
	unsigned int image_length;		//	size of image data, in bytes
	unsigned int file_length;		//	size of entire file, in bytes
	
	Thing base;
	ThingGroup<Male>	males;
	ThingGroup<Female>	females;
	ThingGroup<Item>	items;
	
	Midpoint midpoint;
	
	bool process_image(ifstream &);			//	handle PNG portion of file
	bool process_studio(ifstream &);			//	handle studio portion of file
	
	void process_transforms(TransformGroup *);
	
	void write_studio(ofstream &);
public:
	SceneFile(const string &f);
	bool process();
};

#endif