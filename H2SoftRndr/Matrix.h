#pragma once

#include <stdio.h>
#include <fstream> // for file access
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <cmath>

class Matrix
{
public:

	unsigned m_rowSize;
	unsigned m_colSize;
	std::vector<std::vector<double>> m_matrix;

	Matrix();
	Matrix(unsigned rowSize, unsigned colSize, double initial);
	virtual ~Matrix();

	// Matrix Operations
	Matrix operator+(Matrix &);
	Matrix operator-(Matrix &);
	Matrix operator*(Matrix &);
	Matrix operator+(double scalar);
	Matrix operator-(double scalar);
	Matrix operator*(double scalar);
	Matrix operator/(double scalar);
	Matrix transpose();

	// Aesthetic Methods
	double& operator()(const unsigned &, const unsigned &);
	void print() const;
	unsigned getRows() const;
	unsigned getCols() const;
};

