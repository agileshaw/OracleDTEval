#include "rules.h"

#ifndef _DATAH_
#define _DATAH_

void dataGenerator(int** dataset, Attributes** attributes, RuleTree* tree, int instances, int n_features, int option);

void allPossibleExamples(int** dataset, RuleTree* tree, int n_features, int* index, int* path, int* path_index);

void addExamples(int** dataset, RuleTree* tree, int n_features, int* index, int* path, int* path_index);

void countPath(RuleTree* tree, int* path);

void randomWalk(int** dataset, RuleTree* tree, int index);

void outputDataset(FILE* data_file, int** dataset, Attributes** attributes, int instances, int n_features);

#endif
