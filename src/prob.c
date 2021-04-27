#include "prob.h"

float treeComparison (RuleTree* tree1, RuleTree* tree2, int n_features)
{
    int i, succ = 0, total = 0;
    Queue *t1, *t2;

    Scanner* scanner = malloc(sizeof(Scanner));
    scanner->values = malloc(sizeof(int) * n_features);

    t1 = createQueue();
    t2 = createQueue();
    enqueue(&t1, tree1);
    enqueue(&t2, tree2);
    scanner->ptr1 = t1->first;
    scanner->ptr2 = t2->first;

    for (i = 0; i < n_features; i++)
        scanner->values[i] = -1;

    scanTree(scanner, &succ, &total, n_features);
    printf("Total: %d, Successful: %d\n", total, succ);
    printf("Accuracy: %f\n", (double)succ/(double)total);

    destroyScanner(scanner, n_features);
    destroyQueue(t1);
    destroyQueue(t2);

    return (double)succ/(double)total;
}

void scanTree (Scanner* scanner, int* succ, int* total, int n_features)
{
    int i, count;
    RuleTree *cur1, *cur2, *node;
    Scanner* cur_scanner = malloc(sizeof(Scanner));
    cur_scanner->ptr1 = scanner->ptr1;
    cur_scanner->ptr2 = scanner->ptr2;
    cur_scanner->values = malloc(sizeof(int) * n_features);
    cur1 = (RuleTree*)(cur_scanner->ptr1->data);
    cur2 = (RuleTree*)(cur_scanner->ptr2->data);

    for (i = 0; i < n_features; i++)
        cur_scanner->values[i] = scanner->values[i];
    if (cur1->parent)
        cur_scanner->values[cur1->parent->attri_selected] = cur1->index;

    if (cur1->label == -1 && cur2->label == -1) {
        if (cur1->attri_selected == cur2->attri_selected) {
            count = cur1->children->count;
            cur_scanner->ptr1 = cur1->children->first;
            cur_scanner->ptr2 = cur2->children->first;
            for (i = 0; i < count; i++) {
                node = (RuleTree*)(cur_scanner->ptr1->data);
                printf("Equal: %d -> %d(%d)\n", node->parent->attri_selected, node->attri_selected, node->index);
                scanTree(cur_scanner, succ, total, n_features);
                cur_scanner->ptr1 = cur_scanner->ptr1->next;
                cur_scanner->ptr2 = cur_scanner->ptr2->next;
            }
        } else if (cur_scanner->values[cur2->attri_selected] != -1) {
            cur_scanner->ptr2 = cur2->children->first;
            for (i = 0; i < cur_scanner->values[cur2->attri_selected]; i++)
                cur_scanner->ptr2 = cur_scanner->ptr2->next;
            printf("Exist! Proceeding...(%d = %d)\n", cur2->attri_selected, cur_scanner->values[cur2->attri_selected]);
            scanTree(cur_scanner, succ, total, n_features);
        } else {
            count = cur1->children->count;
            cur_scanner->ptr1 = cur1->children->first;
            for (i = 0; i < count; i++) {
                node = (RuleTree*)(cur_scanner->ptr1->data);
                printf("Not equal: %d -> %d(%d)\n", node->parent->attri_selected, node->attri_selected, node->index);
                scanTree(cur_scanner, succ, total, n_features);
                cur_scanner->ptr1 = cur_scanner->ptr1->next;
            }
        }
    } else if (cur1->label != -1 && cur2->label == -1) {
        cur_scanner->ptr2 = cur2->children->first;
        if (cur_scanner->values[cur2->attri_selected] != -1) {
            for (i = 0; i < cur_scanner->values[cur2->attri_selected]; i++)
                cur_scanner->ptr2 = cur_scanner->ptr2->next;
            printf("Exist! Proceeding...(%d = %d)\n", cur2->attri_selected, cur_scanner->values[cur2->attri_selected]);
            scanTree(cur_scanner, succ, total, n_features);
        } else {
            count = cur2->children->count;
            for (i = 0; i < count; i++) {
                scanTree(cur_scanner, succ, total, n_features);
                cur_scanner->ptr2 = cur_scanner->ptr2->next;
            }
        }
    } else if (cur1->label == -1 && cur2->label != -1) {
        printf("Tree2: %d = %d : %d\n", cur2->parent->attri_selected, cur2->index, cur2->label);
        count = cur1->children->count;
        cur_scanner->ptr1 = cur1->children->first;
        for (i = 0; i < count; i++) {
            scanTree(cur_scanner, succ, total, n_features);
            cur_scanner->ptr1 = cur_scanner->ptr1->next;
        }
    } else if (cur1->label != -1 && cur2->label != -1) {
        printf("Tree1: %d = %d : %d - Tree2: %d = %d : %d\n", cur1->parent->attri_selected, cur1->index, cur1->label, cur2->parent->attri_selected, cur2->index, cur2->label);
        if (cur1->label == cur2->label)
            (*succ)++;
        (*total)++;
    }

    destroyScanner(cur_scanner, n_features);
}

void destroyScanner (Scanner* scanner, int n_features)
{
    if (scanner == NULL)
        return;
    if (scanner->values != NULL)
        free(scanner->values);
    free(scanner);
}
