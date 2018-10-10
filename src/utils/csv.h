/**
   @file

   @brief Structs and functions for managing CSV files.

   Most of the time you will want to use the csv_read() function to read a CSV
   file into a 2-d array structure of strings (struct csv). Then when you're
   done with it, call csv_free().

   To create a csv structure and then csv files use csv_new(), csv_append()
   and csv_write().

   Use csv_at() to get the string value of a particular cell.
   Use csv_to_matrix() to convert a csv consisting of numbers to a matrix of
   doubles. Remember to free your matrix when done with it using matrix_free().

   Everything else provided here is just cute, or perhaps occasionally useful.
*/


#ifndef USEFUL_CSV_H
#define USEFUL_CSV_H

#include <assert.h>
#include <mach/error.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "test.h"
#include "algorithms.h"

/**
   Holds a single row in a csv file
 */

struct csv_row {
	size_t len;
	size_t capacity;
	char ** cells;
};

/**
   Holds an entire csv file, either after reading it in, or for writing it out.
 */

struct csv {
	struct csv_row header;
	size_t len;
	size_t capacity;
	struct csv_row * rows;
};

/**
   Hold a matrix of real-valued numbers.
 */


struct matrix {
	size_t rows;
	size_t cols;
	double * vals;
};


/**
   Used to keep track of type in str_dbl union.
 */

enum val_type {
	dbl,
	str
};

/**
   Type for dataframe cell. Holds either a string or double.
 */

union str_dbl {
	char *str;
	double dbl;
};

/**
   Represents something akin to an R dataframe.
 */

struct dataframe {
	struct csv_row header;
	size_t rows;
	size_t cols;
	enum val_type *type;
	union str_dbl *vals;
};


struct csv csv_read(FILE *f, bool header, char delim);
const char *csv_at(const struct csv *cs, size_t row, size_t col);
void csv_write(FILE *f, const struct csv * cs);
bool csv_isvalid(const struct csv *cs, bool verbose);
void csv_free(struct csv *cs);
struct dataframe csv_to_dataframe(const struct csv *cs,
				const enum val_type col_types[]);
struct dataframe dataframe_new(size_t cols, const char * strings[],
			const enum val_type types[]);
union str_dbl dataframe_at(const struct dataframe *df, size_t row, size_t col);
void dataframe_append(struct dataframe *df, const union str_dbl vals[]);
void dataframe_append_var(struct dataframe *df, ...);
enum val_type dataframe_col_type(struct dataframe *df, size_t col);
void dataframe_write(FILE *f, const struct dataframe * df);
struct  csv dataframe_to_csv(const struct dataframe * df);
struct matrix dataframe_to_matrix(const struct dataframe * df);
void dataframe_free(struct dataframe *df);
struct matrix csv_to_matrix(const struct csv *cs);
double matrix_at(const struct matrix * mat, size_t row, size_t col);
void matrix_set(struct matrix * mat, size_t row, size_t col, double val);
void matrix_free(struct matrix *matrix);


#endif
