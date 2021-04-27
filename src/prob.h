#include "rules.h"

#ifndef _PROBH_
#define _PROBH_

typedef struct Scanner
{
    Node* ptr1;
    Node* ptr2;
    int* values;
} Scanner;

float treeComparison (RuleTree* tree1, RuleTree* tree2, int n_features);

void scanTree (Scanner* scanner, int* succ, int* total, int n_features);

void destroyScanner (Scanner* scanner, int n_features);

#endif
