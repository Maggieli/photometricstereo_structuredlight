#ifndef FUSING_NORMALS_GEOMETRY_H
#define FUSING_NORMALS_GEOMETRY_H

#include <iostream>
#include <fstream>
#include<string>

using namespace std;

void read_focal_length( string &directory, int& x, int& y);
void read_pointdata( string &directory, string& filename);

#endif