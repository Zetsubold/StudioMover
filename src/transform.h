#pragma once

#include <vector>
#include <string>

#include "spatial.h"
#include "point.h"

enum TransformType {
	TR_empty,
	TR_translate,
	TR_rotate_inplace,
	TR_rotate_origin,
	TR_rotate_midpoint,
	TR_scale_inplace,
	TR_scale_origin,
	TR_scale_midpoint,
};

class Transform
{
	TransformType type;
	point P;
public:
	Transform(const TransformType &, const point &);
	~Transform();
	void modify(const point &);
	
	friend class Spatial;
};

class TransformGroup
{
	std::vector<Transform> transforms;
	TransformType last;
public:
	void store(const TransformType &, const point &);
	Transform get_transform(const unsigned int & index);
	std::vector<Transform>::size_type get_size() { return transforms.size(); };
	TransformGroup();

	friend class Spatial;
	
};

TransformGroup * load_transforms(const std::string &);