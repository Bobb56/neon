#ifndef PROGLOADING_H
#define PROGLOADING_H

#include "trees.h"

TreeBuffer createSyntaxTree(char* program, bool free_after);
TreeBuffer createExpressionTree(char* string, bool free_after);
#endif