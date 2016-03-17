//#define ENABLE_DEBUG
/*	******************************************************	*
 *	CS3230 Assignment 1: Find the Center of Mass of Images	*
 *	by Thenaesh Elango (A0124772E)													*
 *	******************************************************	*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <utility>
#include <cmath>

using namespace std;



class Image
{
	// coordinate encapsulation class
public:
	class CMCoordinates
	{
		pair<pair<int, int>, double> coords;

	public:
		CMCoordinates(int x, int y, double val);
		CMCoordinates(pair<int, int> coords, double** arr);
		void init(int x, int y, double val);

	public:
		int getX() const;
		int getY() const;
		double getVal() const;
	};

protected:
	// fields
	double** raw_pixels; // (row, col)
	double* row_folded_pixels; // each row is the sum of the raw pixel rows
	double* col_folded_pixels; // each col is the sum of the raw pixel cols
	double* top_bottom_deltas;
	double* left_right_deltas;
	pair<int, int> center_of_mass; // row, col

	// constructors
public:
	Image();
	virtual ~Image();

	// essential methods
public:
	virtual void readInput();
	virtual void computeCM();
	virtual CMCoordinates getCM() const; // copy CMCoordinates object in full

	// helper methods
protected:
	virtual void foldRow();
	virtual void foldCol();
	virtual void calculateDelta(double const* arr, double* deltas);
	virtual int findMin(double const* arr) const;

	// debug methods
public:
	virtual void dbgPrint() const;
};



int main(int argc, char** argv)
{
	Image img;

	img.readInput();
	img.computeCM();
	Image::CMCoordinates cm = img.getCM();

	int row = cm.getX() + 1;
	int col = cm.getY() + 1;
	double val = cm.getVal();

	cout 	<< row	<< " "
				<< col	<< " "
				<< setprecision(2)	<< fixed // ensure output is in 2 d.p.
				<< val
				<< endl;
}



Image::CMCoordinates::CMCoordinates(int x, int y, double val)
{
	this->init(x, y, val);
}

Image::CMCoordinates::CMCoordinates(pair<int, int> coords, double** arr)
{
	this->init(coords.first, coords.second, arr[coords.first][coords.second]);
}

void Image::CMCoordinates::init(int x, int y, double val)
{
	this->coords.first.first = x;
	this->coords.first.second = y;
	this->coords.second = val;
}


int Image::CMCoordinates::getX() const
{
	return this->coords.first.first;
}

int Image::CMCoordinates::getY() const
{
	return this->coords.first.second;
}

double Image::CMCoordinates::getVal() const
{
	return this->coords.second;
}


Image::Image()
{
	// initialise all the pixel arrays
	this->row_folded_pixels = new double[25];
	this->col_folded_pixels = new double[25];
	this->raw_pixels = new double*[25];
	for (int i = 0; i < 25; i++) this->raw_pixels[i] = new double[25];

	// initialise delta arrays
	this->top_bottom_deltas = new double[25];
	this->left_right_deltas = new double[25];
}

Image::~Image()
{
	delete this->left_right_deltas;
	delete this->top_bottom_deltas;

	for (int i = 0; i < 25; i++) delete this->raw_pixels[i];
	delete this->raw_pixels;
	delete this->col_folded_pixels;
	delete this->row_folded_pixels;
}


void Image::readInput()
{
	for (int i = 0; i < 25; i++)
		for (int j = 0; j < 25; j++)
			cin >> this->raw_pixels[i][j];
}

void Image::computeCM()
{
	// compute folded rows and cols
	this->foldRow(); // O(n^2) time
	this->foldCol(); // O(n^2) time

	// calculate delta arrays in O(n^2) time each
	this->calculateDelta(this->row_folded_pixels, this->top_bottom_deltas);
	this->calculateDelta(this->col_folded_pixels, this->left_right_deltas);

	// get furthest minimal row and col - this is the furthest CM
	int cm_row = this->findMin(this->top_bottom_deltas);
	int cm_col = this->findMin(this->left_right_deltas);

	// store computed CM
	this->center_of_mass.first = cm_row;
	this->center_of_mass.second = cm_col;
}

Image::CMCoordinates Image::getCM() const
{
#ifdef ENABLE_DEBUG
	this->dbgPrint();
#endif

	CMCoordinates cm_coords(this->center_of_mass, this->raw_pixels);
	return cm_coords;
}


void Image::foldRow()
{
	/*
	 * i: row
	 * j: col
	 */
	for (int i = 0; i < 25; i++)
	{
		this->row_folded_pixels[i] = 0;
		for (int j = 0; j < 25; j++)
			this->row_folded_pixels[i] += this->raw_pixels[i][j];
	}
}

void Image::foldCol()
{
	/*
	 * i: row
	 * j: col
	 */
	for (int j = 0; j < 25; j++)
	{
		this->col_folded_pixels[j] = 0;
		for (int i = 0; i < 25; i++)
			this->col_folded_pixels[j] += this->raw_pixels[i][j];
	}
}

void Image::calculateDelta(double const* arr, double* deltas)
{
	for (int r = 0; r < 25; r++)
	{
		double first_sum = 0, second_sum = 0;

		for (int i = 0; i < r; i++) first_sum += arr[i];
		for (int i = r + 1; i < 25; i++) second_sum += arr[i];

		deltas[r] = abs(first_sum - second_sum);
	}
}

int Image::findMin(double const* arr) const
{
	// max possible delta value is 625, so this is safe
	double min_val = static_cast<double>(1<<30);
	int min_idx = -1;

	for (int i = 0; i < 25; i++)
		if (arr[i] <= min_val) // <= to get largest-indexed minimal element
		{
			min_idx = i;
			min_val = arr[i];
		}

	return min_idx;
}


void Image::dbgPrint() const
{
	cout << "START OF TEST" << endl;
	cout << setprecision(2) << fixed;

	// print whole pixel matrix
	cout << endl;
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			cout << this->raw_pixels[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	// print folded row
	cout << endl;
	for (int i = 0; i < 25; i++)
	{
		cout << this->row_folded_pixels[i] << endl;
	}
	cout << endl;

	// print folded col
	cout << endl;
	for (int j = 0; j < 25; j++)
	{
		cout << this->col_folded_pixels[j] << " ";
	}
	cout << endl;

	// print top-bottom deltas
	cout << endl;
	for (int i = 0; i < 25; i++)
	{
		cout << this->top_bottom_deltas[i] << endl;
	}
	cout << endl;

	// print left-right deltas
	cout << endl;
	for (int j = 0; j < 25; j++)
	{
		cout << this->left_right_deltas[j] << " ";
	}
	cout << endl;

	cout << endl;
	cout << this->top_bottom_deltas[this->findMin(this->top_bottom_deltas)] << endl;
	cout << this->left_right_deltas[this->findMin(this->left_right_deltas)] << endl;
	cout << endl;

	cout << "END OF TEST" << endl;
}
