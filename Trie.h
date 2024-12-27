#pragma once
#include "WordInDocument.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
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
    ~Trie();
    void insert(const std::string& word, const std::string& docName, int pos);
    std::vector<WordInDocument> search(const std::string& word);
    void dump(const std::string& filePath);
    void load(const std::string& filePath);
	void dumpHelper(TrieNode* node, std::ofstream& file);
	void loadHelper(TrieNode* node, std::ifstream& file);
	void sort_trie(Trie& trie);
};

TrieNode::TrieNode() : isEndOfWord(false) {}
Trie::Trie() {
	root = new TrieNode();
}
Trie::~Trie() {
	delete root;
}
void Trie::insertHelper(TrieNode* node, const std::string& word, const std::string& docName, int pos) {
	if (word.empty()) {
		node->isEndOfWord = true;
		for (WordInDocument& doc : node->documents) {
			if (doc.documentName == docName) {
				doc.addPosition(pos);
				return;
			}
		}
		WordInDocument newDoc(docName);
		newDoc.addPosition(pos);
		node->documents.push_back(newDoc);
		return;
	}
	char firstChar = word[0];
	if (node->children.find(firstChar) == node->children.end()) {
		node->children[firstChar] = new TrieNode();
	}
	insertHelper(node->children[firstChar], word.substr(1), docName, pos);
}
void Trie::insert(const std::string& word, const std::string& docName, int pos) {
	insertHelper(root, word, docName, pos);
}
void Trie::searchHelper(TrieNode* node, const std::string& word, std::vector<WordInDocument>& results) {
	if (word.empty()) {
		if (node->isEndOfWord) {
			results = node->documents;
		}
		return;
	}
	char firstChar = word[0];
	if (node->children.find(firstChar) == node->children.end()) {
		return;
	}
	searchHelper(node->children[firstChar], word.substr(1), results);
}
std::vector<WordInDocument> Trie::search(const std::string& word) {
	std::vector<WordInDocument> results;
	searchHelper(root, word, results);
	return results;
}
void Trie::dump(const std::string& filePath) {
	std::ofstream file(filePath, std::ios::binary);
	dumpHelper(root, file);
	file.close();
}
void Trie::load(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary);
	loadHelper(root, file);
	file.close();
}
/*
void Trie::dumpHelper(TrieNode* node, std::ofstream& file) {
	int childrenCount = node->children.size();
	file.write(reinterpret_cast<char*>(&childrenCount), sizeof(int));
	for (const auto& child : node->children) {
		file.write(&child.first, sizeof(char));
		file.write(reinterpret_cast<char*>(&child.second->isEndOfWord), sizeof(bool));
		int docCount = child.second->documents.size();
		file.write(reinterpret_cast<char*>(&docCount), sizeof(int));
		for (const WordInDocument& doc : child.second->documents) {
			int docNameSize = doc.documentName.size();
			file.write(reinterpret_cast<char*>(&docNameSize), sizeof(int));
			file.write(doc.documentName.c_str(), docNameSize);
			int posCount = doc.positions.size();
			file.write(reinterpret_cast<char*>(&posCount), sizeof(int));
			for (int pos : doc.positions) {
				file.write(reinterpret_cast<char*>(&pos), sizeof(int));
			}
		}
		dumpHelper(child.second, file);
	}
}
void Trie::loadHelper(TrieNode* node, std::ifstream& file) {
	int childrenCount;
	file.read(reinterpret_cast<char*>(&childrenCount), sizeof(int));
	for (int i = 0; i < childrenCount; ++i) {
		char firstChar;
		file.read(&firstChar, sizeof(char));
		node->children[firstChar] = new TrieNode();
		file.read(reinterpret_cast<char*>(&node->children[firstChar]->isEndOfWord), sizeof(bool));
		int docCount;
		file.read(reinterpret_cast<char*>(&docCount), sizeof(int));
		for (int j = 0; j < docCount; ++j) {
			int docNameSize;
			file.read(reinterpret_cast<char*>(&docNameSize), sizeof(int));
			std::string docName(docNameSize, ' ');
			file.read(&docName[0], docNameSize);
			int posCount;
			file.read(reinterpret_cast<char*>(&posCount), sizeof(int));
			WordInDocument doc(docName);
			for (int k = 0; k < posCount; ++k) {
				int pos;
				file.read(reinterpret_cast<char*>(&pos), sizeof(int));
				doc.addPosition(pos);
			}
			node->children[firstChar]->documents.push_back(doc);
		}
		loadHelper(node->children[firstChar], file);
	}
}
*/
void Trie::sort_trie(Trie& trie) {
	//based on occurence
	std::vector<WordInDocument> temp;
	for (auto& child : root->children) {
		for (auto& doc : child.second->documents) {
			temp.push_back(doc);
		}
	}
	std::sort(temp.begin(), temp.end(), [](const WordInDocument& a, const WordInDocument& b) {
		return a.positions.size() > b.positions.size();
		});
	for (auto& child : root->children) {
		child.second->documents = temp;
	}
}

////////////////////////////////////////////
void Trie::dumpHelper(TrieNode* node, std::ofstream& file) {
	file << node->children.size() << '\n'; // Write number of children
	for (const auto& child : node->children) {
		file << child.first << '\n'; // Write character key
		file << child.second->isEndOfWord << '\n'; // Write isEndOfWord flag
		file << child.second->documents.size() << '\n'; // Write number of documents

		for (const WordInDocument& doc : child.second->documents) {
			file << doc.documentName << '\n'; // Write document name
			file << doc.positions.size(); // Write number of positions
			for (int pos : doc.positions) {
				file << ' ' << pos; // Write positions as space-separated values
			}
			file << '\n'; // End positions
		}
		dumpHelper(child.second, file); // Recursive call
	}
}

void Trie::loadHelper(TrieNode* node, std::ifstream& file) {
	int childrenCount;
	file >> childrenCount; // Read number of children
	file.ignore(); // Skip newline

	for (int i = 0; i < childrenCount; ++i) {
		char firstChar;
		file >> firstChar; // Read character key
		file.ignore(); // Skip newline

		TrieNode* childNode = new TrieNode();
		node->children[firstChar] = childNode;

		bool isEndOfWord;
		file >> isEndOfWord; // Read isEndOfWord flag
		childNode->isEndOfWord = isEndOfWord;
		file.ignore(); // Skip newline

		int docCount;
		file >> docCount; // Read number of documents
		file.ignore(); // Skip newline

		for (int j = 0; j < docCount; ++j) {
			std::string docName;
			std::getline(file, docName); // Read document name

			int posCount;
			file >> posCount; // Read number of positions
			file.ignore(); // Skip space or newline

			WordInDocument doc(docName);
			for (int k = 0; k < posCount; ++k) {
				int pos;
				file >> pos; // Read position
				doc.addPosition(pos);
			}
			file.ignore(); // Skip newline at the end of positions

			childNode->documents.push_back(doc);
		}

		loadHelper(childNode, file); // Recursive call
	}
}