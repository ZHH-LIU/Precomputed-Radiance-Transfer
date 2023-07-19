//////////////////////////////////////////////////////////////////////////////////////////
//	SH Rotation.h
//	Functions to rotate SH coefficients
//	Downloaded from: www.paulsprojects.net
//	Created:	2nd October 2003
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef SH_ROTATION_H
#define SH_ROTATION_H

void RotateSHCoefficients(int numBands, float* unrotatedCoeffs, float* rotatedCoeffs,
	float theta, float phi);

//Get a matrix for rotating one band of coefficients about the Z axis
void GetZRotationMatrix(int band, float* entries, float angle);

//Get a matrix for rotating one band of coefficients about the X axis, by 90 degrees
void GetX90DegreeRotationMatrix(int band, float* entries);

//Apply a matrix to a vector of coefficients
void ApplyMatrix(int size, float* matrix, bool transpose,
	float* inVector, float* outVector);
#endif