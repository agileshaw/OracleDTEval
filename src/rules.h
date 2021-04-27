#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "node.h"

#ifndef _RULESH_
#define _RULESH_

typedef struct Attributes
{
	char* name;
	char* values[1000];
    int num_values;
} Attributes;

typedef struct RuleTree
{
	struct RuleTree* parent;
	Queue* children;
	int level;
	int index;
	int label;
    int total_attris;
	int* avail_attris;
	int attri_selected;
} RuleTree;

RuleTree* ruleGenerator(Attributes** attributes, int n_features, int depth);

RuleTree* createRuleTree(Attributes** attributes, int n_features);

RuleTree* addRule(RuleTree* parent, Attributes** attributes, int level, int index, int n_features);

void expandTree(RuleTree* parent, Attributes** attributes, int n_features, int depth);

void freeAttributes(Attributes **attributes, int n_features);

void printRuleTree(RuleTree* tree, Attributes** attributes, int* leaf, int* size, int* attris);

void printTreeToFile(FILE* file, RuleTree* tree, Attributes** attributes);

RuleTree* readTreeFile (FILE *file, Attributes** attributes, int n_features);

RuleTree* addChild(RuleTree* node, char* str, Attributes** attributes, int n_features, int flag);

void destroyRuleTree(RuleTree* tree);

unsigned seedCurTime();

char* removeExtra (const char* str, int* level, int* flag);

char* correctValue(const char* str, int* level, int flag);

char* readClassLabel(const char* str, int* level);

int reachLeaf(const char* str);

#endif
