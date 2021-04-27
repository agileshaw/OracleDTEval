#include "data.h"

void dataGenerator(int** dataset, Attributes** attributes, RuleTree* tree, int instances, int n_features, int option)
{
    int i, j, k, path = 0, index = 0, tmp, count = 0;

    if (option == 1) { // generate uniquely random dataset
        int *path_index = malloc(sizeof(int)*(int)(pow(2, n_features)));
        int **complete = malloc(sizeof(int *)*(int)(pow(2, n_features)));

        for (i = 0; i < (int)(pow(2, n_features)); i++) {
            complete[i] = malloc(sizeof(int)*(n_features+1));
            for (j = 0; j <= n_features; j++)
                complete[i][j] = -1;
        }

        allPossibleExamples(complete, tree, n_features, &index, &path, path_index);
        if ((instances < index) && (instances >= path)) {
            for (i = 1; i <= path; i++) {
                tmp = rand()/(RAND_MAX/(int)(pow(2, n_features)));
                while (path_index[tmp] != i)
                    tmp = rand()/(RAND_MAX/(int)(pow(2, n_features)));
                for (j = 0; j <= n_features; j++)
                    dataset[count][j] = complete[tmp][j];
                path_index[tmp] = 0;
                count++;
            }
            while(count < instances) {
                tmp = rand()/(RAND_MAX/(int)(pow(2, n_features)));
                while (path_index[tmp] == 0)
                    tmp = rand()/(RAND_MAX/(int)(pow(2, n_features)));
                for (j = 0; j <= n_features; j++)
                    dataset[count][j] = complete[tmp][j];
                path_index[tmp] = 0;
                count++;
            }
        } else if ((instances < index) && (instances < path)) {
            k = path - instances;
            for (i = 1; i <= path && count < instances; i++) {
                if (k) {
                    tmp = rand()/(RAND_MAX/2);
                    if (tmp == 1) {
                        i++;
                        k--;
                    }
                }
                tmp = rand()/(RAND_MAX/(int)(pow(2, n_features)));
                while (path_index[tmp] != i)
                    tmp = rand()/(RAND_MAX/(int)(pow(2, n_features)));
                for (j = 0; j <= n_features; j++)
                    dataset[count][j] = complete[tmp][j];
                path_index[tmp] = 0;
                count++;
            }
        }

        for (i = 0; i < (int)(pow(2, n_features)); i++)
            free(complete[i]);
        free(complete);
    } else if (option == 0) { //generate completely random dataset
        for (i = 0; i < instances; i++)
            randomWalk(dataset, tree, i);

        for (i = 0; i < instances; i++) {
            for (j = 0; j < n_features; j++) {
                if (dataset[i][j] == -1) {
                    dataset[i][j] = rand()/(RAND_MAX/(attributes[j]->num_values));
                }
            }
        }
    }
}

void allPossibleExamples(int** dataset, RuleTree* tree, int n_features, int* index, int* path, int* path_index)
{
    int i, j;
    Node* cur = tree->children->first;
	RuleTree* node;
    int *tmp = malloc(sizeof(int)*n_features);

    for (j = 0; j < tree->children->count; j++) {
        node = (RuleTree*)(cur->data);
        if(j) {
            for (i = 0; i < n_features; i++)
                dataset[*index][i] = tmp[i];
        }
        dataset[*index][node->parent->attri_selected] = node->index;
        if (node->label != -1) {
            dataset[*index][node->attri_selected] = node->label;
            for (i = 0; i < n_features; i++)
                tmp[i] = dataset[*index][i];
            (*path)++;
            addExamples(dataset, tree, n_features, index, path, path_index);
        } else {
            for (i = 0; i < n_features; i++)
                tmp[i] = dataset[*index][i];
            allPossibleExamples(dataset, node, n_features, index, path, path_index);
        }
        cur = cur->next;
    }
    free(tmp);
}

void addExamples(int** dataset, RuleTree* tree, int n_features, int* index, int* path, int* path_index)
{
    int i, j, num, count = 0, missing = 0;
    int *ref;

    for (i = 0; i < n_features; i++) {
        if (dataset[*index][i] == -1)
            missing++;
    }

    if (missing > 0) {
        num = (int)(pow(2, missing));
        for (i = 1; i < num; i++) {
            for (j = 0; j <= n_features; j++)
                dataset[(*index+i)][j] = dataset[*index][j];
        }
        ref = malloc(sizeof(int)*missing);
        memset(ref, 0, sizeof(int)*missing);
        for (i = 0; i < num; i++) {
            path_index[*index] = *path;
            for (j = 0; j < n_features; j++) {
                if (dataset[*index][j] == -1) {
                    if(i % (int)(pow(2, count)) == 0)
                        ref[count] = 1 - ref[count];
                    dataset[*index][j] = ref[count];
                    count++;
                }
            }
            count = 0;
            (*index)++;
        }
    } else {
        (*index)++;
    }
}

void countPath(RuleTree* tree, int* path)
{
    int i;
    Node* cur = tree->children->first;
	RuleTree* node;

    for (i = 0; i < tree->children->count; i++) {
        node = (RuleTree*)(cur->data);
        if (node->label != -1)
            (*path)++;
        else
            countPath(tree, path);
        cur = cur->next;
    }
}

void randomWalk(int** dataset, RuleTree* tree, int index)
{
    int i, tmp;
    Node* cur = tree->children->first;
	RuleTree* node;

    tmp = rand()/(RAND_MAX/(tree->children->count));
    for (i = 0; i < tmp; i++)
        cur = cur->next;

    node = (RuleTree*)(cur->data);
    dataset[index][node->parent->attri_selected] = node->index;

    if (node->label != -1)
        dataset[index][node->attri_selected] = node->label;
    else
        randomWalk(dataset, node, index);
}

void outputDataset(FILE* data_file, int** dataset, Attributes** attributes, int instances, int n_features)
{
    int i, j;

    for (j = 0; j < n_features; j++) {
        fprintf(data_file, "%s,", attributes[j]->name);
    }
    fprintf(data_file, "%s\n", attributes[j]->name);

    for (i = 0; i < instances; i++) {
        for (j = 0; j < n_features; j++) {
            if (dataset[i][j] != -1)
                fprintf(data_file, "%s,", attributes[j]->values[dataset[i][j]]);
            else
                fprintf(data_file, ",");
        }

        fprintf(data_file, "%s\n", attributes[j]->values[dataset[i][n_features]]);
    }
}
