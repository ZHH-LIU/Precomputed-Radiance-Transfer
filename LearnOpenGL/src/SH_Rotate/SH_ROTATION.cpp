#include "SH_ROTATION.h"
#include "iostream"
void RotateSHCoefficients(int numBands, float* unrotatedCoeffs, float* rotatedCoeffs, float theta, float phi)
{
	int numFunctions = numBands * numBands;

	for (int i = 0; i < numFunctions; ++i)
		rotatedCoeffs[i] = unrotatedCoeffs[i];

	//Band 0 coefficient is unchanged
	rotatedCoeffs[0] = unrotatedCoeffs[0];

	//Rotate band 1 coefficients
	if (numBands > 1)
	{
		//Get the rotation matrices for band 1 (want to apply Z1*Xt*Z2*X)
		float band1X[9];
		float band1Z1[9];
		float band1Z2[9];

		GetZRotationMatrix(1, band1Z1, phi);
		GetX90DegreeRotationMatrix(1, band1X);
		GetZRotationMatrix(1, band1Z2, theta);

		//Create space to hold the intermediate results
		float band1A[3], band1B[3], band1C[3];

		//Apply the matrices
		ApplyMatrix(3, band1X, false, &unrotatedCoeffs[1], band1A);
		ApplyMatrix(3, band1Z2, false, band1A, band1B);
		ApplyMatrix(3, band1X, true, band1B, band1C);

		ApplyMatrix(3, band1Z1, false, band1C, &rotatedCoeffs[1]);
	}

	//Rotate band 2 coefficients
	if (numBands > 2)
	{
		float band2X[25];
		float band2Z1[25];
		float band2Z2[25];

		GetZRotationMatrix(2, band2Z1, phi);
		GetX90DegreeRotationMatrix(2, band2X);
		GetZRotationMatrix(2, band2Z2, theta);

		//Create space to hold the intermediate results
		float band2A[5], band2B[5], band2C[5];

		//Apply the matrices
		ApplyMatrix(5, band2X, false, &unrotatedCoeffs[4], band2A);
		ApplyMatrix(5, band2Z2, false, band2A, band2B);
		ApplyMatrix(5, band2X, true, band2B, band2C);

		ApplyMatrix(5, band2Z1, false, band2C, &rotatedCoeffs[4]);
	}

	//Rotate band 3 coefficients
	if (numBands > 3)
	{
		float band3X[49];
		float band3Z1[49];
		float band3Z2[49];

		GetZRotationMatrix(3, band3Z1, phi);
		GetX90DegreeRotationMatrix(3, band3X);
		GetZRotationMatrix(3, band3Z2, theta);

		//Create space to hold the intermediate results
		float band3A[7], band3B[7], band3C[7];

		//Apply the matrices
		ApplyMatrix(7, band3X, false, &unrotatedCoeffs[9], band3A);
		ApplyMatrix(7, band3Z2, false, band3A, band3B);
		ApplyMatrix(7, band3X, true, band3B, band3C);

		ApplyMatrix(7, band3Z1, false, band3C, &rotatedCoeffs[9]);
	}

	//Rotate band 3 coefficients
	if (numBands > 4)
	{
		float band4X[81];
		float band4Z1[81];
		float band4Z2[81];

		GetZRotationMatrix(4, band4Z1, phi);
		GetX90DegreeRotationMatrix(4, band4X);
		GetZRotationMatrix(4, band4Z2, theta);

		//Create space to hold the intermediate results
		float band4A[9], band4B[9], band4C[9];

		//Apply the matrices
		ApplyMatrix(9, band4X, false, &unrotatedCoeffs[16], band4A);
		ApplyMatrix(9, band4Z2, false, band4A, band4B);
		ApplyMatrix(9, band4X, true, band4B, band4C);

		ApplyMatrix(9, band4Z1, false, band4C, &rotatedCoeffs[16]);
	}
}

void GetZRotationMatrix(int band, float* entries, float angle)
{
	//Calculate the size of the matrix
	int size = 2 * band + 1;

	//Convert angle to radians
	const float PI = 3.14159265359;
	angle *= PI / 180.0;

	//Entry index
	int currentEntry = 0;

	//Loop through the rows and columns of the matrix
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j, ++currentEntry)
		{
			//Initialise this entry to zero
			entries[currentEntry] = 0.0;

			//For the central row (i=(size-1)/2), entry is 1 if j==i, else zero
			if (i == (size - 1) / 2)
			{
				if (j == i)
					entries[currentEntry] = 1.0;

				continue;
			}

			//For i<(size-1)/2, entry is cos if j==i or sin if j==size-i-1
			//The angle used is k*angle where k=(size-1)/2-i
			if (i < (size - 1) / 2)
			{
				int k = (size - 1) / 2 - i;

				if (j == i)
					entries[currentEntry] = cos(k * angle);

				if (j == size - i - 1)
					entries[currentEntry] = sin(k * angle);

				continue;
			}

			//For i>(size-1)/2, entry is cos if j==i or -sin if j==size-i-1
			//The angle used is k*angle where k=i-(size-1)/2
			if (i > (size - 1) / 2)
			{
				int k = i - (size - 1) / 2;

				if (j == i)
					entries[currentEntry] = cos(k * angle);

				if (j == size - i - 1)
					entries[currentEntry] = -sin(k * angle);

				continue;
			}
		}
	}

	return;
}

void GetX90DegreeRotationMatrix(int band, float* entries)
{
	//Ensure that 0<=band<=3
	if(band>4)
	{
		std::cout<<"X rotation matrices are only known for bands 0-3";
		return;
	}

	if(band==0)
	{
		entries[0]= 1.0;
	}

	if(band==1)
	{
		entries[0]= 0.0;
		entries[1]= 1.0;
		entries[2]= 0.0;
		entries[3]=-1.0;
		entries[4]= 0.0;
		entries[5]= 0.0;
		entries[6]= 0.0;
		entries[7]= 0.0;
		entries[8]= 1.0;
	}

	if(band==2)
	{
		entries[ 0]= 0.0;
		entries[ 1]= 0.0;
		entries[ 2]= 0.0;
		entries[ 3]= 1.0;
		entries[ 4]= 0.0;
		entries[ 5]= 0.0;
		entries[ 6]=-1.0;
		entries[ 7]= 0.0;
		entries[ 8]= 0.0;
		entries[ 9]= 0.0;
		entries[10]= 0.0;
		entries[11]= 0.0;
		entries[12]=-0.5;
		entries[13]= 0.0;
		entries[14]=-sqrt(3.0)/2;
		entries[15]=-1.0;
		entries[16]= 0.0;
		entries[17]= 0.0;
		entries[18]= 0.0;
		entries[19]= 0.0;
		entries[20]= 0.0;
		entries[21]= 0.0;
		entries[22]=-sqrt(3.0)/2;
		entries[23]= 0.0;
		entries[24]= 0.5;
	}

	if(band==3)
	{
		//Initialise all entries to 0
		for(int i=0; i<49; ++i)
			entries[i]=0.0;

		entries[ 3]=-sqrt(0.625);
		entries[ 5]= sqrt(0.375);
		
		entries[ 8]=-1.0;
		
		entries[17]=-sqrt(0.375);
		entries[19]=-sqrt(0.625);
		
		entries[21]= sqrt(0.625);
		entries[23]= sqrt(0.375);
		
		entries[32]=-0.25;
		entries[34]=-sqrt(15.0)/4;
		
		entries[35]=-sqrt(0.375);
		entries[37]= sqrt(0.625);

		entries[46]=-sqrt(15.0)/4;
		entries[48]= 0.25;
	}

	if (band == 4)
	{
		for (int i = 0; i < 81; ++i)
			entries[i] = 0.0;

		float temp[81] = { 0.0, 1.0, 0, 0, 0, 0, 0, 0, 0 ,
			1, 0, 0, 0, 0, 0, 0, 0,0,
			0, 0, 1, 0, 0, 0, 0, 0, 0,
			-2.714114, -2.554753, -0.037868, -4.372322, -3.341168, -3.332327, -2.882963, -0.359954, -0.26581,
			0, 0, 0, 0, 0, 1,0, 0, 0,
			3.318313, 1.040246, 0.629931, 3.907173, 2.871077, 3.543243, 3.340196, 0.417045, 0.307970,
			2.728679, -1.297534, -2.441133, -3.637524, -3.603918, -4.010915, -3.109682, -1.388262, -0.286712,
			0.115988, 2.197645, -0.032265, 2.847259, 2.820956, 1.041447, 1.434093, 0.303910, 0.224422,
			-4.839294, -2.741847, -3.781884, -6.908838, -6.845011, -8.124372, -5.906294, -0.737435, -1.544559 };
		entries = temp;
	}

	return;
}

void ApplyMatrix(int size, float* matrix, bool transpose, float* inVector, float* outVector)
{
	//Loop through entries
	for (int i = 0; i < size; ++i)
	{
		//Clear this entry of outVector
		outVector[i] = 0.0;

		//Loop through matrix row/column
		for (int j = 0; j < size; ++j)
		{
			if (transpose)
				outVector[i] += matrix[j * size + i] * inVector[j];
			else
				outVector[i] += matrix[i * size + j] * inVector[j];
		}
	}
}
