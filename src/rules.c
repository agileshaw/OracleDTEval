#include "rules.h"

RuleTree* ruleGenerator(Attributes** attributes, int n_features, int depth)
{
	int random, leaf = 0, size = 0;
	RuleTree* ruletree = createRuleTree(attributes, n_features);
	int *attris;

	if(ruletree->total_attris != 0) {
		random = rand()/(RAND_MAX/n_features);
		ruletree->attri_selected = random;
		printf("Root random: %d\n", random);
		if (depth > 1)
			expandTree(ruletree, attributes, n_features, depth);
	}

	attris = malloc(sizeof(int) * n_features);
	memset(attris, 0, n_features*sizeof(attris[0]));

	printf("\n----------------Rule Tree----------------\n");
	printRuleTree(ruletree, attributes, &leaf, &size, attris);
	printf("\nNumber of leaves: %d\n", leaf);
	printf("Size of the tree: %d\n\n", size);

	return ruletree;
}

RuleTree* createRuleTree(Attributes** attributes, int n_features)
{
    return addRule(NULL, attributes, 0, 0, n_features);
}

RuleTree* addRule(RuleTree* parent, Attributes** attributes, int level, int index, int n_features)
{
	int i;
    RuleTree* node = malloc(sizeof(RuleTree));

	node->parent = parent;
	node->children = createQueue();
	node->level = level;
	node->index = index;
	node->label = -1;
	node->total_attris = 0;
	node->avail_attris = malloc(sizeof(int) * n_features);

	for (i = 0; i < n_features; i++) {
		node->avail_attris[i] = 1;
		node->total_attris++;
	}
	if (parent) {
		for (i = 0; i < n_features; i++) {
			if (parent->avail_attris[i] != 1 || parent->attri_selected == i) {
				node->avail_attris[i] = 0;
				node->total_attris--;
			}
		}
	}

    return node;
}

void expandTree(RuleTree* parent, Attributes** attributes, int n_features, int depth)
{
	int i, random;
	RuleTree* node, *tmp;

	for (i = 0; i < attributes[parent->attri_selected]->num_values; i++) {
		node = addRule(parent, attributes, parent->level+1, i, n_features);
		if (node->level < depth && node->total_attris > 0) {
				random = rand()/(RAND_MAX/n_features);
				while (node->avail_attris[random] == 0)
					random = rand()/(RAND_MAX/n_features);
				node->attri_selected = random;
				enqueue(&(parent->children), (void *)node);
				expandTree(node, attributes, n_features, depth);
		} else {
			node->attri_selected = n_features;
			if(parent->children->count != 0) {
				tmp = (RuleTree*)(parent->children->first->data);
				node->label = 1- tmp->label;
			} else
				node->label = rand()/(RAND_MAX/2);
			enqueue(&(parent->children), (void *)node);
		}
	}
}

void printRuleTree(RuleTree* tree, Attributes** attributes, int* leaf, int* size, int* attris)
{
    int i, j;
	Node* cur = tree->children->first;
	RuleTree* node;

    for (j = 0; j < tree->children->count; j++) {
		node = (RuleTree*)(cur->data);

		if (!(node->parent)) {
			cur = cur->next;
			node =(RuleTree*)(cur->data);
			j++;
			(*size)++;
		}

		for (i = 1; i < node->level; i++)
			printf("|   ");

		printf("%s = %s", attributes[node->parent->attri_selected]->name, attributes[node->parent->attri_selected]->values[node->index]);
		attris[node->parent->attri_selected] = 1;

		if (node->label != -1) {
			(*leaf)++;
			printf(": %s\n", attributes[node->attri_selected]->values[node->label]);
		} else
			printf("\n");

		(*size)++;
		printRuleTree(node, attributes, leaf, size, attris);
		cur = cur->next;
	}
}

void printTreeToFile(FILE* file, RuleTree* tree, Attributes** attributes)
{
	int i, j;
	Node* cur = tree->children->first;
	RuleTree* node;

    for (j = 0; j < tree->children->count; j++) {
		node = (RuleTree*)(cur->data);

		if (!(node->parent)) {
			cur = cur->next;
			node =(RuleTree*)(cur->data);
			j++;
		}

		for (i = 1; i < node->level; i++)
			fprintf(file, "|   ");

		fprintf(file, "%s = %s", attributes[node->parent->attri_selected]->name, attributes[node->parent->attri_selected]->values[node->index]);

		if (node->label != -1)
			fprintf(file, ": %s\n", attributes[node->attri_selected]->values[node->label]);
		else
			fprintf(file, "\n");

		printTreeToFile(file, node, attributes);
		cur = cur->next;
	}
}

RuleTree* readTreeFile (FILE *file, Attributes** attributes, int n_features)
{
    RuleTree *node = createRuleTree(attributes, n_features);
	RuleTree *tree = node;
    char *tmp, *new_tmp, *tok;
    char line[1024];
	int i, level, last = -1, count = 0, weka = 0, flag;

	while (fgets(line, 2048, file)) {
		if (line[0] == '-' || line[0] == 'R') {
			printf("WEKA!\n");
			weka = 1;
			break;
		} else
			count++;
}

	fseek(file, 0, SEEK_SET);
	printf("%d\n", count);
	count += 2;

	if (weka == 1) {
		while (count > 0) {
			fgets(line, 2048, file);
			count--;
		}
	}

	while (fgets(line, 2048, file)) {
		if (line[0] != '\n') {
			tmp = strdup(line);
            tok = strtok(tmp, "=");
			flag = 0;
			new_tmp = removeExtra(tok, &level, &flag);
			if (level >= last) {
				for (i = 0; i < n_features; i++) {
					if (!strcmp(new_tmp, attributes[i]->name))
						node->attri_selected = i;
				}
				for (i = 0; i < attributes[node->attri_selected]->num_values; i++) {
					RuleTree *new_node = addRule(node, attributes, node->level+1, i, n_features);
					enqueue(&(node->children), (void *)new_node);
				}
				tok = strtok(NULL, "=");
				node = addChild(node, tok, attributes, n_features, flag);
			} else if (level < last) {
				while(node->level > level)
					node = node->parent;
				tok = strtok(NULL, "=");
				node = addChild(node, tok, attributes, n_features, flag);
			}
			last = node->level;
			free(tmp);
			free(new_tmp);
		} else
			break;
	}

    return tree;
}

RuleTree* addChild(RuleTree* node, char* str, Attributes** attributes, int n_features, int flag)
{
	int i, level;
	char *new_tmp, *tmp, *leaf_token;
	Node *cur;

	if (reachLeaf(str)) {
		leaf_token = strtok(str, ":");
		new_tmp = correctValue(leaf_token, &level, flag);
		cur = node->children->first;
		i = 0;
		while (strcmp(new_tmp, attributes[node->attri_selected]->values[i]) != 0) {
			cur = cur->next;
			i++;
		}
		node = (RuleTree*)(cur->data);
		leaf_token = strtok(NULL, ":");
		tmp = readClassLabel(leaf_token, &level);
		node->attri_selected = n_features;
		for (i = 0; i < attributes[node->attri_selected]->num_values; i++) {
			if (!strcmp(tmp, attributes[node->attri_selected]->values[i]))
				node->label = i;
		}
		free(tmp);
		free(new_tmp);
	} else {
		new_tmp = correctValue(str, &level, flag);
		cur = node->children->first;
		i = 0;
		while (strcmp(new_tmp, attributes[node->attri_selected]->values[i]) != 0) {
			cur = cur->next;
			i++;
		}
		node = (RuleTree*)(cur->data);
		free(new_tmp);
	}

	return node;
}

void destroyRuleTree(RuleTree* tree)
{
    if (tree == NULL)
		return;

	RuleTree* node;

	while ((node = dequeue(tree->children)) != NULL)
		destroyRuleTree(node);
	destroyQueue(tree->children);

    if (tree != NULL) {
        if (tree->avail_attris != NULL)
			free(tree->avail_attris);
		free(tree);
    }
}

unsigned seedCurTime()
{
	unsigned seed = 0;
	time_t cur_time = time(0);
	unsigned char *tmp;
	size_t i;

	tmp = (unsigned char *)&cur_time;
	for (i = 0; i < sizeof(cur_time); i++)
		seed = seed *(UCHAR_MAX) + tmp[i];

	return seed;
}

char* removeExtra (const char* str, int* level, int* flag)
{
	int i = 0, j = 0;
    char *new_str = (char *)malloc(100);

	(*level) = 0;
    while ((str[i] != '\0') && (str[i] != '\n')) {
		if (str[i] == '|') {
			i++;
			(*level)++;
		}
		if (str[i] == '(')
			break;
		if (str[i] == '!') {
			(*flag)++;
			i++;
		}
	    if (str[i] != ' ') {
			new_str[j] = str[i];
			j++;
		}
	    i++;
    }
    new_str[j] = '\0';

    return new_str;
}

char* correctValue(const char* str, int* level, int flag)
{
	int i = 0, j = 0;
    char *new_str = (char *)malloc(100);

	(*level) = 0;
    while ((str[i] != '\0') && (str[i] != '\n')) {
		if (str[i] == '(')
			i++;
		if (str[i] == ')') {
			break;
		}
	    if (str[i] != ' ') {
			if(!flag) {
				new_str[j] = str[i];
			} else {
				if (str[i] == 'T')
					new_str[j] = 'F';
				else if (str[i] == 'F')
					new_str[j] = 'T';
			}
			j++;
		}
	    i++;
    }
    new_str[j] = '\0';

    return new_str;
}

char* readClassLabel(const char* str, int* level)
{
	int i = 0, j = 0;
    char *new_str = (char *)malloc(100);

	(*level) = 0;
    while ((str[i] != '\0') && (str[i] != '\n')) {
		if (str[i] == '|') {
			i++;
			(*level)++;
		}
		if (str[i] == '(')
			break;
	    if (str[i] != ' ') {
			if (str[i] == '1' || str[i] == 'T')
				new_str[j] = 'T';
			else if (str[i] == '0' || str[i] == 'F')
				new_str[j] = 'F';
			j++;
		}
	    i++;
    }
    new_str[j] = '\0';

    return new_str;
}

int reachLeaf(const char* str)
{
	int i = 0, count = 0;

	while (str[i] != '\0') {
		if (str[i] == ':')
			count++;
		i++;
	}

	return count;
}