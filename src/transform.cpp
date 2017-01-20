#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <iomanip>

#include "transform.h"
#include "logging.h"

using namespace std; 

Transform::Transform(const TransformType & t, const point & p) : type(t), P(p)
{
}

Transform::~Transform()
{
}

void Transform::modify(const point & p)
{
	if (type == TR_translate) 
		P += p;
	else
	if ( (type == TR_scale_inplace) || (type == TR_scale_midpoint) || (type == TR_scale_origin) )
		P *= p;
}


TransformGroup::TransformGroup() : last(TR_empty)
{
}

Transform TransformGroup::get_transform(const unsigned int & index)
{
	if ((index>=0) && (index<transforms.size()))
		return transforms[index];
	else
		return Transform(TR_empty, point(1.0, 1.0, 1.0));
}

void TransformGroup::store(const TransformType & t, const point & p)
{
	if (t==last) {
		if ( ( t!=TR_rotate_inplace ) && ( t!=TR_rotate_origin ) && ( t!=TR_rotate_midpoint ) ) {
			transforms.back().modify(p);
			
			logfile << "	modified by " << p.x << ", " << p.y << ", " << p.z << endl;
		}
	}
	else {
		transforms.push_back( Transform(t, p) );
		
		logfile << "added [" << t << "] " << p.x << ", " << p.y << ", " << p.z << endl;
		
		//	set last to current type
		last = t;
	}
}


float get_float(istringstream & stream)
{
	float f;
	
	stream >> f;
	
	//	skip over any comma delimiters after a float
	if (stream.peek() == ',')
	stream.seekg(1, ios_base::cur);
	
	return f;
}

bool valid_type (const string & token)
{
	return ( token=="TRANSLATE" ) || ( token=="ROTATE" ) || ( token=="SCALE" );
}

bool valid_subtype (const string & token)
{
	return ( token=="ORIGIN" ) || ( token=="MIDPOINT" );
}

bool valid_axis (const string & token)
{
	return ( token=="X" ) || ( token=="Y" ) || ( token=="Z" );
}

void store_axis (const string & axis, const float & f, const TransformType trtype, TransformGroup * tgroup)
{
	if ( (trtype==TR_translate) ||
		 (trtype==TR_rotate_inplace) || (trtype==TR_rotate_origin) || (trtype==TR_rotate_midpoint) )
	{
		if 		(axis=="X")		tgroup->store(trtype, point(f, 0.0, 0.0));
		else if (axis=="Y")		tgroup->store(trtype, point(0.0, f, 0.0));
		else if (axis=="Z")		tgroup->store(trtype, point(0.0, 0.0, f));
	}
	else {
		if 		(axis=="X")		tgroup->store(trtype, point(f, 1.0, 1.0));
		else if (axis=="Y")		tgroup->store(trtype, point(1.0, f, 1.0));
		else if (axis=="Z")		tgroup->store(trtype, point(1.0, 1.0, f));
	}

}

static void convert_uppercase (string & s)
{
	for (string::iterator iter=s.begin(); iter!=s.end(); iter++)
		*iter = toupper(*iter);
}


TransformGroup * load_transforms(const std::string & filename)
{
	ifstream file(filename);
	
	if (!file) {
		logfile << "load_transforms -- failed to load proper file." << endl;
		return nullptr;
	}
	
	TransformGroup * tgroup = new TransformGroup;
	
	while (!file.eof()) {
		
		/*	token #1 - [ TRANSLATE | ROTATE | SCALE ]
		 * 	token #2 - [ X | Y | Z ] OR [ ORIGIN | MIDPOINT ] OR float (the first of three)
		 * 	token #3 - [ X | Y | Z ] OR float (the first of three)
		 *  token #4 - float (the first of three)
		 * 
		 *  getting an axis designator also triggers getting the required float
		 *  ROTATE requires an axis designator
		 */
		 
		string line;
		getline(file, line);
		
		//	skip empty lines
		if (line.length() == 0)
			continue;
		
		istringstream stream(line);
		
		string token;
	
		//	-- TOKEN #1 --
		stream >> token;
		convert_uppercase(token);
		
		
		//	skip comments
		if ( token.find("##") != string::npos )
			continue;
		
		if ( !valid_type(token) ) {
			cout << "the following StudioMover.txt line doesn't start with a valid token\n";
			cout << line << endl;
			logfile << "load_transforms -- \"" << token << "\" is not a valid transform type." << nouppercase << endl;
			continue;
		}
		
		TransformType trtype = (token=="TRANSLATE") ? TR_translate : (token=="ROTATE") ? TR_rotate_inplace : TR_scale_inplace;

		//	-- TOKEN #2 --
		
		//	save stream's position in case we need to re-read the next token
		istringstream::pos_type pos = stream.tellg();
		
		stream >> token;
		convert_uppercase(token);
		
		bool skip_to_floats = false;
		
		if ( valid_axis(token) ) {
			float f = get_float(stream);
			
			store_axis(token, f, trtype, tgroup);
		
			//	finished with this line
			continue;
		}
		else if ( valid_subtype(token) ) {
			if (trtype==TR_translate) {
				//	log a message and then ignore subtype
				logfile << "no subtype needed for translate -- " << line << endl;
			}
			else if (trtype==TR_rotate_inplace) {
				if		(token=="ORIGIN") 	trtype = TR_rotate_origin;
				else						trtype = TR_rotate_midpoint;
			}
			else {
				if		(token=="ORIGIN") 	trtype = TR_scale_origin;
				else						trtype = TR_scale_midpoint;
			}
		}
		else {
			//  cannot rotate without an axis
			if (trtype==TR_rotate_inplace) {
				logfile << "rotate needs an axis of rotation -- " << line << endl;
				continue;
			}
			
			//	not an axis or subtype - must be floats
			stream.seekg(pos);
			skip_to_floats = true;
		}
		
		
		if (!skip_to_floats) {
			//	save stream's position again
			pos = stream.tellg();

			//	-- TOKEN #3 --
			stream >> token;
			convert_uppercase(token);
			
			if (valid_axis(token)) {
				float f = get_float(stream);
				
				store_axis(token, f, trtype, tgroup);
			
				//	finished with this line
				continue;
			}
			else {
			//  cannot rotate without an axis
				if ( (trtype==TR_rotate_inplace) || (trtype==TR_rotate_origin) || (trtype==TR_rotate_midpoint) ) {
					logfile << "rotate needs an axis of rotation -- " << line << endl;
					continue;
				}
				
				//	reset token
				stream.seekg(pos);
			}
		}
		
		float f1,f2,f3;

		stream >> f1;
		
		if (stream.peek() == ',')
			stream.seekg(1, ios_base::cur);

		stream >> f2;

		if (stream.peek() == ',')
			stream.seekg(1, ios_base::cur);

		stream >> f3;
		
		tgroup->store(trtype, point(f1, f2, f3));
		
	}
	
	return tgroup;
}