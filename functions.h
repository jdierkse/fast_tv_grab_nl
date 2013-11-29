#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <string>
#include <vector>


int OutputProgress(int currentItem, int totalItems, int previousPercentage);
std::vector<std::string> SplitString(std::string string, std::string delimiter);
std::string FixHTMLAndSpecialCharacters(std::string string);

#endif // FUNCTIONS_H

