#include "main.h"

int main(int argc, char *argv[])
{
    int i, j, opt, export_data = 0, compare = 0, option = 0;
    int n_features = 5, max_values = 2, depth = 5, instances = 20;
    int ofile = 0, dfile = 0, pfile = 0;
    double acc;
    unsigned seed = seedCurTime();
    char s[20], f[10], v[10], d[10], n[10], o[10], p[PATHLEN];
    char oraclefile[PATHLEN], datafile[PATHLEN], probfile[PATHLEN];
    int** dataset;
    Attributes **attributes;
    RuleTree* tree, *new_tree;

    printf("Generated Seed: %u\n", seed);
    while ((opt = getopt(argc, argv, "s:f:v:d:n:p:a:t:z:o:")) != -1) {
        switch (opt) {
        case 's':
            snprintf(s, 20, "%s", optarg);
            seed = strtoul(s, NULL, 10);
            printf("The input seed: %u\n", seed);
            break;
        case 'f':
            snprintf(f, 10, "%s", optarg);
            n_features = strtol(f, NULL, 10);
            break;
        case 'v':
            snprintf(v, 10, "%s", optarg);
            max_values = strtol(v, NULL, 10);
            break;
        case 'd':
            snprintf(d, 10, "%s", optarg);
            depth = strtol(d, NULL, 10);
            break;
        case 'n':
            export_data = 1;
            snprintf(n, 10, "%s", optarg);
            instances = strtol(n, NULL, 10);
            break;
        case 'p':
            compare = 1;
            snprintf(p, 200, "%s", optarg);
            break;
        case 'a':
            dfile = 1;
            snprintf(datafile, PATHLEN, "%s", optarg);
            break;
        case 't':
            ofile = 1;
            snprintf(oraclefile, PATHLEN, "%s", optarg);
            break;
        case 'z':
            pfile = 1;
            snprintf(probfile, PATHLEN, "%s", optarg);
            break;
        case 'o':
            snprintf(o, 10, "%s", optarg);
            option = strtol(o, NULL, 10);
            break;
        case '?':
            write(STDOUT_FILENO, "Usage: ./generator [-s seed] [-f n_features] [-v max_values] [-d depth] [-n n_instances] [-p oraclefile] [-a dataset_path] [-t oracle_path] [-z result_path] [-o dataset_type]\n", 175);
            exit(1);
        }
    }

    if (depth > n_features) {
        printf("Depth cannot be larger than the number of features!\n");
        exit(1);
    } else if (depth < 1 || n_features < 1) {
        printf("Cannot generate rules with the given arguments!\n");
        exit(1);
    } else if (max_values < 2 || max_values > 1000) {
        printf("Invalid number of values!\n");
        exit(1);
    }

    if (!ofile) {
        printf("Oracle path needs to be specified!\n");
        exit(1);
    } else if (export_data && !compare && !dfile) {
        printf("Dataset path needs to be specified!\n");
        exit(1);
    } else if (compare && !pfile) {
        printf("Result path needs to be specified!\n");
        exit(1);
    }

    if (option != 0 && option != 1) {
        printf("The option variable only accept 0 or 1 as input!\n");
        exit(1);
    }

    srand(seed);
    attributes = malloc(sizeof(Attributes *) * (n_features + 1));
    for (i = 0; i < n_features; i++) {
        attributes[i] = malloc(sizeof(Attributes));
        attributes[i]->name = malloc(10 * sizeof(char));
        snprintf(attributes[i]->name, 10, "F%d", i);
        if (max_values == 2) {
            attributes[i]->num_values = 2;
            attributes[i]->values[0] = malloc(3 * sizeof(char));
            attributes[i]->values[1] = malloc(3 * sizeof(char));
            snprintf(attributes[i]->values[0], 3, "F");
            snprintf(attributes[i]->values[1], 3, "T");
            printf("%s: %s, %s\n", attributes[i]->name, attributes[i]->values[0], attributes[i]->values[1]);
        } else {
            printf("%s: ", attributes[i]->name);
            attributes[i]->num_values = rand()/(RAND_MAX/(max_values+1));
            printf("%d\n", attributes[i]->num_values);
            while (attributes[i]->num_values < 2)
                attributes[i]->num_values = rand()/(RAND_MAX/(max_values+1));
            for (j = 0; j < attributes[i]->num_values; j++) {
                attributes[i]->values[j] = malloc(10 * sizeof(char));
                snprintf(attributes[i]->values[j], 10, "V%d", j);
                printf("%s ", attributes[i]->values[j]);
            }
            printf("\n");
        }
    }

    attributes[n_features] = malloc(sizeof(Attributes));
    attributes[n_features]->name = malloc(10 * sizeof(char));
    snprintf(attributes[n_features]->name, 10, "Class");
    attributes[n_features]->values[0] = malloc(3 * sizeof(char));
    attributes[n_features]->values[1] = malloc(3 * sizeof(char));
    snprintf(attributes[n_features]->values[0], 3, "F");
    snprintf(attributes[n_features]->values[1], 3, "T");
    attributes[n_features]->num_values = 2;
    printf("%s: %s, %s\n", attributes[n_features]->name, attributes[n_features]->values[0], attributes[n_features]->values[1]);

    tree = ruleGenerator(attributes, n_features, depth);
    snprintf(oraclefile+strlen(oraclefile), PATHLEN-strlen(oraclefile), "/oracle-s%u-f%d-v%d-d%d", seed, n_features, max_values, depth);
    FILE *oracle_file = fopen(oraclefile, "w+");
    if(oracle_file == NULL) {
        perror("Unable to open oracle_file! -1");
        exit(1);
    }

    printTreeToFile(oracle_file, tree, attributes);
    fclose(oracle_file);

    if (export_data && !compare) {
        dataset = malloc(sizeof(int *)*instances);
        for (i = 0; i < instances; i++) {
            dataset[i] = malloc(sizeof(int)*(n_features+1));
            for (j = 0; j <= n_features; j++)
                dataset[i][j] = -1;
        }
        dataGenerator(dataset, attributes, tree, instances, n_features, option);

        snprintf(datafile+strlen(datafile), PATHLEN-strlen(datafile), "/train-s%u-f%d-v%d-d%d-n%d.csv", seed, n_features, max_values, depth, instances);
        FILE *data_file = fopen(datafile, "w+");
        if(data_file == NULL) {
            perror("Unable to open data_file! -2");
            exit(1);
        }
        outputDataset(data_file, dataset, attributes, instances, n_features);
        fclose(data_file);
    }

    if (compare) {
        FILE *f = fopen(p, "r+");
        if(f == NULL) {
            perror("Unable to open file! -3");
            exit(1);
        }

        new_tree = readTreeFile(f, attributes, n_features);

        acc = treeComparison(tree, new_tree, n_features);

        if (!export_data)
            snprintf(probfile+strlen(probfile), PATHLEN-strlen(probfile), "/prob-f%d-v%d-d%d", n_features, max_values, depth);
        else
            snprintf(probfile+strlen(probfile), PATHLEN-strlen(probfile), "/prob-f%d-v%d-d%d-n%d", n_features, max_values, depth, instances);

        FILE *prob_file = fopen(probfile, "a+");
        if(prob_file == NULL) {
            perror("Unable to open file! -4");
            exit(1);
        }
        fprintf(prob_file, "%f\n", acc);

        destroyRuleTree(new_tree);
        fclose(f);
        fclose(prob_file);
    }

    freeAttributes(attributes, n_features);
    destroyRuleTree(tree);

    return 0;
}

void freeAttributes(Attributes **attributes, int n_features)
{
    int i, j;

    for (i = 0; i <= n_features; i++) {
        if (attributes[i]->name) {
            free(attributes[i]->name);
            for (j = 0; j < attributes[i]->num_values; j++) {
				free(attributes[i]->values[j]);
			}
        }
        free(attributes[i]);
    }
    free(attributes);
}
