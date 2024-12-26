#pragma once
#include "WordInDocument.h"
#include <unordered_map>
#include <unordered_map>
#include <vector>
#include <string>
#include "WordInDocument.h"

struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    std::vector<WordInDocument> documents;

    TrieNode();
};

class Trie {
private:
    TrieNode* root;

    void insertHelper(TrieNode* node, const std::string& word, const std::string& docName, int pos);
    void searchHelper(TrieNode* node, const std::string& word, std::vector<WordInDocument>& results);

public:
    Trie();
    void insert(const std::string& word, const std::string& docName, int pos);
    std::vector<WordInDocument> search(const std::string& word);
};

TrieNode::TrieNode() : isEndOfWord(false) {}

Trie::Trie() : root(new TrieNode()) {}

void Trie::insertHelper(TrieNode* node, const std::string& word, const std::string& docName, int pos) {
    for (char ch : word) {
        if (!node->children[ch])
            node->children[ch] = new TrieNode();
        node = node->children[ch];
    }
    node->isEndOfWord = true;

    // Add document and position
    bool docExists = false;
    for (auto& doc : node->documents) {
        if (doc.documentName == docName) {
            doc.addPosition(pos);
            docExists = true;
            break;
        }
    }
    if (!docExists) {
        node->documents.emplace_back(docName); // Add the document
        node->documents.back().addPosition(pos); // Add the position to the last element
    }
}

void Trie::insert(const std::string& word, const std::string& docName, int pos) {
    insertHelper(root, word, docName, pos);
}

void Trie::searchHelper(TrieNode* node, const std::string& word, std::vector<WordInDocument>& results) {
    for (char ch : word) {
        if (!node->children[ch]) return;
        node = node->children[ch];
    }
    if (node->isEndOfWord) results = node->documents;
}

std::vector<WordInDocument> Trie::search(const std::string& word) {
    std::vector<WordInDocument> results;
    searchHelper(root, word, results);
    return results;
}

