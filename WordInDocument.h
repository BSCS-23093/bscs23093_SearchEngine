#pragma once
#include <string>
#include <vector>

class WordInDocument {
public:
    std::string documentName;
    std::vector<int> positions;
    
    WordInDocument(const std::string& docName);
    void addPosition(int pos);
};
WordInDocument::WordInDocument(const std::string& docName) : documentName(docName) {}

void WordInDocument::addPosition(int pos) {
    positions.push_back(pos);
}