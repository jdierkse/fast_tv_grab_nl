#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <string>
#include <vector>


int OutputProgress(int currentItem, int totalItems, int previousPercentage);
std::vector<std::string> SplitString(const std::string& string, const std::string& delimiter);
std::string FixHTMLAndSpecialCharacters(const std::string& string);

#endif // FUNCTIONS_H

