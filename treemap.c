#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) 
{
    TreeMap * new = (TreeMap *)malloc(sizeof(TreeMap));
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    return new;
    //new->lower_than = lower_than;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) 
{
   TreeNode * newNode = createTreeNode(key, value);
    if (newNode == NULL) {
        return;
    }

    // Si el árbol está vacío, el nuevo nodo será la raíz
    if (tree->root == NULL) {
        tree->root = newNode;
        return;
    }

    TreeNode * parent = NULL;
    TreeNode * current = tree->root;
    while (current != NULL) {
        parent = current;
        if (is_equal(tree, key, current->pair->key)) {
            free(newNode->pair);
            free(newNode);
            return;
        } else if (tree->lower_than(key, current->pair->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    newNode->parent = parent;

    if (tree->lower_than(key, parent->pair->key)) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    } 

    tree->current = newNode;
}



TreeNode * minimum(TreeNode * x){
    if (x == NULL) return NULL;
    while (x->left != NULL) 
    {
        x = x->left;
    }
    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) 
{
    if (tree == NULL || node == NULL) {
        return;
    }
    TreeNode * parent = node->parent;
    if (node->left == NULL && node->right == NULL) {
        if (parent == NULL) {
            tree->root = NULL;  
        } else if (node == parent->left) {
            parent->left = NULL;
        } else {
            parent->right = NULL;
        }
        free(node->pair);
        free(node);
    }
    else if (node->left == NULL || node->right == NULL) {
        TreeNode * child = (node->left != NULL) ? node->left : node->right;
        if (parent == NULL) {
            tree->root = child;
            child->parent = NULL;
        } else if (node == parent->left) {
            parent->left = child;
            child->parent = parent;
        } else {
            parent->right = child;
            child->parent = parent;
        }
        free(node->pair);
        free(node);
    }
    else 
    {
        TreeNode * successor = minimum(node->right);
        if (successor->parent != node) 
        {
            successor->parent->left = successor->right;
            if (successor->right != NULL) {
                successor->right->parent = successor->parent;
                }
            successor->right = node->right;
            node->right->parent = successor;
        }
        successor->left = node->left;
        node->left->parent = successor;

        if (parent == NULL) {
            tree->root = successor;
            successor->parent = NULL;
        } else if (node == parent->left) {
            parent->left = successor;
            successor->parent = parent;
        } else {
            parent->right = successor;
            successor->parent = parent;
            }

        free(node->pair);
        free(node);
    }
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}




Pair * searchTreeMap(TreeMap * tree, void* key) 
{
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * aux = tree->root;
    while (aux != NULL)
        {
            if (is_equal(tree, key, aux->pair->key) == 1)
            {
                tree->current = aux;
                return aux->pair;
            }
            aux = (tree->lower_than(key, aux->pair->key) == 1) ? aux->left : aux->right;
        }
    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) 
{
    TreeNode * aux = tree->root;
    TreeNode * ub = NULL;
    while (aux != NULL)
        {
            if (is_equal(tree, key, aux->pair->key) == 1)
            {
                ub = aux;
                aux = aux->right;
            }
            else
            {
                if (tree->lower_than(key, aux->pair->key) == 1)
                {
                    aux = aux->left;
                }
                else
                {
                    ub = aux;
                    aux = aux->right;
                }
            }
        }
    if (ub != NULL)
    {
        tree->current = ub;
        return ub->pair;
    }
    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) 
{
    if (tree == NULL || tree->root == NULL) return NULL;
    tree->current = minimum(tree->root);
    return tree->current->pair;
    
}

Pair * nextTreeMap(TreeMap * tree) 
{
    TreeNode * current = tree->current;

    // Caso 1: Si el nodo tiene un subárbol derecho, el sucesor es el mínimo del subárbol derecho
    if (current->right != NULL) {
        tree->current = minimum(current->right);
        return tree->current->pair;
        }

    // Caso 2: Si el nodo no tiene subárbol derecho, el sucesor es el primer ancestro derecho
    TreeNode * parent = current->parent;
    while (parent != NULL && current == parent->right) {
        current = parent;
        parent = parent->parent;
    }

    // parent es el siguiente nodo (sucesor), o NULL si current era el máximo nodo
    tree->current = parent;
    return (parent != NULL) ? parent->pair : NULL;

}
