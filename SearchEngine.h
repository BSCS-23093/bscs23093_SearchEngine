#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "Trie.h"
#include <filesystem>
#include <mutex>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <Windows.h>
std::mutex mapMutex;

class SearchEngine {
private:
    std::unordered_map<std::string, std::vector<WordInDocument>> hashMap;
    Trie trie;

    void parseFile(const std::string& filePath, const std::string& docName);
	void parseFile_trie(const std::string& filePath, const std::string& docName);

public:
    void Search_hashmap_1word(SearchEngine& engine, const std::string& query);
    //
	void addNewDocument_trie(const std::string& filePath);
	void addNewDocument_hashMap(const std::string& filePath);
    //
    void crawlDocuments(const std::string& folderPath);
    std::vector<WordInDocument> searchWithHashMap(const std::string& query);
    void addNewDocument(const std::string& filePath);
    void dumpSearchEngine(const std::string& filePath);
    void loadSearchEngine(const std::string& filePath);
    void print_loaded_searchengine(SearchEngine& engine);
    void Search_loaded_engine_hashmap(SearchEngine& engine, const std::string& query);
    void Search_loaded_engine_hashmap_words_2(SearchEngine& engine, const std::string& query1, const std::string& query2);
    void sort_engine_hashmap(SearchEngine& engine);
    void Search_loaded_engine_hashmap_words_multiple(SearchEngine& engine, const std::string& query);
    void Search_loaded_engine_hashmap_subtraction(SearchEngine& engine, const std::string& query1, const std::string& query2);
    void Search_loaded_engine_hashmap_sentence(SearchEngine& engine, const std::string& query);
    //void seaarch_loaded_engine_trie(SearchEngine& engine, const std::string& query);
    void processFileBatch(const std::vector<std::string>& fileBatch);
    void crawlDocumentsParallel(const std::string& folderPath, size_t totalFiles, size_t numThreads);
    void parseFileBuffered(const std::string& filePath, const std::string& docName,
        std::unordered_map<std::string, std::vector<WordInDocument>>& localMap);
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<WordInDocument> searchWithTrie(const std::string& query);
	void trie_dump(const std::string& filePath);
	void trie_load(const std::string& filePath);
	void crawlDocuments_trie(const std::string& folderPath);
    void search_trie_print(const std::string&folderPath,std::string query);
    void sort_engine_trie(SearchEngine& engine);
    void search_trie_print_2_words(const std::string& folderPath, std::string query);
    void search_trie_print_multiple(const std::string& folderPath, std::string query);
    void search_trie_subtraction(const std::string& folderPath, std::string query);
    void search_trie_sentence(const std::string& folderPath, std::string query);
};
/*
void SearchEngine::parseFile(const std::string& filePath, const std::string& docName) {
    std::ifstream file(filePath);
    std::string word;
    int position = 0;

    while (file >> word) {
        position++;
        // Check if the word is in the hash map
        if (hashMap.find(word) == hashMap.end()) {
            // If not, create a new entry
            hashMap[word] = { WordInDocument(docName) };
            hashMap[word][0].addPosition(position);
        }
        else {
            auto& docList = hashMap[word];
            auto it = std::find_if(docList.begin(), docList.end(),
                [&](const WordInDocument& doc) { return doc.documentName == docName; });

            if (it != docList.end()) {
                // If the document already exists, update its positions
                it->addPosition(position);
            }
            else {
                // Otherwise, create a new WordInDocument entry for this document
                docList.emplace_back(docName);
                docList.back().addPosition(position);
            }
        }

        // Trie insertion remains unchanged
        trie.insert(word, docName, position);
    }
    file.close();
}
*/

void SearchEngine::parseFile(const std::string& filePath, const std::string& docName) {
    const size_t bufferSize = 16 * 1024; // 16 KB buffer size
    char buffer[bufferSize];

    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::string currentWord;
    int position = 0;

    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        size_t bytesRead = file.gcount();
        for (size_t i = 0; i < bytesRead; ++i) {
            char c = buffer[i];

            // Cast 'c' to unsigned char before passing to isspace or ispunct
            if (std::isspace(static_cast<unsigned char>(c)) || std::ispunct(static_cast<unsigned char>(c))) {
                if (!currentWord.empty()) {
                    position++;

                    // Add word to hash map
                    if (hashMap.find(currentWord) == hashMap.end()) {
                        hashMap[currentWord] = { WordInDocument(docName) };
                        hashMap[currentWord][0].addPosition(position);
                    }
                    else {
                        auto& docs = hashMap[currentWord];
                        if (docs.empty() || docs.back().documentName != docName) {
                            docs.emplace_back(docName);
                        }
                        docs.back().addPosition(position);
                    }

                    currentWord.clear();
                }
            }
            else {
                currentWord += c;
            }
        }
    }

    // Add any remaining word at the end of the buffer
    if (!currentWord.empty()) {
        position++;
        if (hashMap.find(currentWord) == hashMap.end()) {
            hashMap[currentWord] = { WordInDocument(docName) };
            hashMap[currentWord][0].addPosition(position);
        }
        else {
            auto& docs = hashMap[currentWord];
            if (docs.empty() || docs.back().documentName != docName) {
                docs.emplace_back(docName);
            }
            docs.back().addPosition(position);
        }
    }

    file.close();
}

void SearchEngine::crawlDocuments(const std::string& folderPath) {
	time_t start, end;
	time(&start);
    for (int i = 1; i < 20000; ++i) {
        std::string filePath = folderPath + "/review_" + std::to_string(i) + ".txt";
        std::ifstream file(filePath);
        if (!file.is_open()) break;

        parseFile(filePath, "review_" + std::to_string(i));
    }
	time(&end);
	std::cout << "Time taken to crawl documents: " << difftime(end, start) << " seconds\n";
    //
    /*
    time(&start);
    for (int i = 1001; i < 2000; ++i) {
        std::string filePath = folderPath + "/review_" + std::to_string(i) + ".txt";
        std::ifstream file(filePath);
        if (!file.is_open()) break;

        parseFile(filePath, "review_" + std::to_string(i));
    }
    time(&end);
    std::cout << "Time taken to crawl documents: " << difftime(end, start) << " seconds\n";
    */
}

std::vector<WordInDocument> SearchEngine::searchWithHashMap(const std::string& query) {
    if (hashMap.find(query) != hashMap.end()) {
        return hashMap[query];
    }
    return {};
}

std::vector<WordInDocument> SearchEngine::searchWithTrie(const std::string& query) {
    //return trie.search(query);
	std::vector<WordInDocument> results;
    results = trie.search(query);
    //sort reults
	std::sort(results.begin(), results.end(),
		[](const WordInDocument& a, const WordInDocument& b) {
			return a.positions.size() > b.positions.size();
		});
	return results;
}

void SearchEngine::addNewDocument(const std::string& filePath) {
    parseFile(filePath, filePath);
}
/*
void SearchEngine::dumpSearchEngine(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for dumping data." << std::endl;
        return;
    }

    for (const auto& pair : hashMap) {
        file << pair.first; // Write the word
        for (const auto& doc : pair.second) {
            file << " " << doc.documentName; // Write the document name
            for (int pos : doc.positions) {
                file << " " << pos; // Write the positions
            }
        }
        file << " .\n"; // Add the delimiter at the end of each word's data
    }

    file.close();
    std::cout << "Search engine data dumped successfully to " << filePath << "." << std::endl;
}
*/
/*
void SearchEngine::dumpSearchEngine(const std::string& filePath) {
	std::ofstream file(filePath);
    for (const auto& pair : hashMap) {
        file << pair.first << " ";
        for (const auto& doc : pair.second) {
            file << doc.documentName << " ";
            for (int pos : doc.positions) {
                file << pos << " ";
            }
        }
        file << "\n";
    }
}
*/
void SearchEngine::dumpSearchEngine(const std::string& filePath) {
    std::ofstream file(filePath);
    for (const auto& pair : hashMap) {
        file << pair.first << " ";
        for (const auto& doc : pair.second) {
            file << doc.documentName << " ";
            for (int pos : doc.positions) {
                file << pos << " ";
            }
			file << "-1 ";
        }
        file << "\n";
    }
}
/*
void SearchEngine::loadSearchEngine(const std::string& filePath) {
    std::ifstream rdr(filePath);
    if (!rdr.is_open()) {
        std::cerr << "Error: Unable to open file for loading data." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(rdr, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        std::vector<WordInDocument> docs;
        std::string docName;
        while (iss >> docName) {
            WordInDocument doc(docName);
            int pos;
            while (iss >> pos) {
                doc.addPosition(pos);
                if (iss.peek() == ' ' || iss.peek() == EOF) break;
            }
            docs.push_back(doc);
        }
        hashMap[word] = docs;
    }

    rdr.close();
    std::cout << "Search engine data loaded successfully from " << filePath << "." << std::endl;
}
*/
void SearchEngine::loadSearchEngine(const std::string& filePath) {
    time_t start, end;
    time(&start);
    std::ifstream rdr(filePath);
    if (!rdr.is_open()) {
        std::cerr << "Error: Unable to open file for loading data." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(rdr, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        std::vector<WordInDocument> docs;
        std::string docName;
        while (iss >> docName ) {
                WordInDocument doc(docName);
                int pos;
                while (iss >> pos && pos != -1) {
                    doc.addPosition(pos);
                    //if (iss.peek() == ' ' || iss.peek() == '\n' || iss.peek() == EOF)break;
                }
                docs.push_back(doc);
		}
        hashMap[word] = docs;
    }

    rdr.close();
    std::cout << "Search engine data loaded successfully from " << filePath << "." << std::endl;
	time(&end);
	std::cout << "Time taken to load documents: " << difftime(end, start) << " seconds\n";
}
/*
void SearchEngine::loadSearchEngine(const std::string& filePath) {
    std::ifstream rdr(filePath);
    if (!rdr.is_open()) {
        std::cerr << "Error: Unable to open file for loading data." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(rdr, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        std::vector<WordInDocument> docs;
        std::string docName;
        while (iss >> docName) {
            WordInDocument doc(docName);
            int pos;
            while (iss >> pos) {
                if (iss.peek() == ' ' || iss.peek() == EOF) {
                    doc.addPosition(pos);
                } else {
                    iss.unget();
                    break;
                }
            }
            docs.push_back(doc);
        }
        hashMap[word] = docs;
    }

    rdr.close();
    std::cout << "Search engine data loaded successfully from " << filePath << "." << std::endl;
}
*/

////////////////////////////////////////////////
/*
void SearchEngine::loadSearchEngine(const std::string& filePath) {
	std::ifstream rdr(filePath);
	std::string word;
	while (rdr >> word) {
		std::string docName;
		rdr >> docName;
		if (hashMap.find(word) == hashMap.end()) {
			hashMap[word] = { WordInDocument(docName) };
		}
		else {
			auto& docList = hashMap[word];
			auto it = std::find_if(docList.begin(), docList.end(),
				[&](const WordInDocument& doc) { return doc.documentName == docName; });

			if (it != docList.end()) {
				it->positions.clear();
			}
			else {
				docList.emplace_back(docName);
			}
		}

		int pos;
		while (rdr >> pos) {
			hashMap[word].back().addPosition(pos);
		}
	}
}
*/
/*
void SearchEngine::loadSearchEngine(const std::string& filePath) {
    std::ifstream rdr(filePath);
    if (!rdr.is_open()) {
        std::cerr << "Error: Unable to open file for loading data." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(rdr, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word; // Read the word

        std::vector<WordInDocument> docs;

        std::string docName;
        while (iss >> docName) {
            WordInDocument doc(docName);

            int pos;
            while (iss >> pos) {
                if (iss.peek() == '\n' ) break; // End of line or document positions
                doc.addPosition(pos);
            }

            // Add the document to the vector of documents for this word
            docs.push_back(doc);
        }

        // Assign the vector of documents to the hash map
        hashMap[word] = docs;
    }

    rdr.close();
    std::cout << "Search engine data loaded successfully from " << filePath << "." << std::endl;
}
*/
void SearchEngine::print_loaded_searchengine(SearchEngine& engine) {
	for (const auto& pair : engine.hashMap) {
		std::cout << pair.first << " ";
		for (const auto& doc : pair.second) {
			std::cout << doc.documentName << " ";
			for (int pos : doc.positions) {
				std::cout << pos << " ";
			}
            std::cout << "(Count: " << doc.positions.size() << ") ";
		}
		std::cout << "\n";
	}
}
/*
void SearchEngine::Search_loaded_engine_hashmap(SearchEngine& engine, const std::string& query) {
    if (engine.hashMap.find(query) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[query]) {
            std::cout << doc.documentName << " ";
            for (int pos : doc.positions) {
                std::cout << pos << " ";
            }
            std::cout << "\n";
        }
    }
}
*/

void SearchEngine::Search_loaded_engine_hashmap(SearchEngine& engine, const std::string& query) {
    if (engine.hashMap.find(query) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[query]) {
            std::cout << doc.documentName << " ";
            for (int pos : doc.positions) {
                std::cout << pos << " ";
                // Implementation to get first 5 and last 5 words from the position of the selected word and print them
                std::ifstream file("review_text/"+doc.documentName + ".txt");
                if (file.is_open()) {
                    std::string word;
                    std::vector<std::string> words;
                    int currentPos = 0;

                    while (file >> word) {
                        currentPos++;
                        if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                            words.push_back(word);
                        }
                        if (currentPos > pos + 5) {
                            break;
                        }
                    }

                    // Print the words
                    for (const auto& w : words) {
                        std::cout << w << " ";
                    }
                    file.close();
                } else {
                    std::cerr << "Error: Could not open file " << doc.documentName << std::endl;
                }
                //
            }
            std::cout << "(Count: " << doc.positions.size() << ") ";
            std::cout << "\n";
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
/*
void SearchEngine::parseFile2(const std::string& filePath, const std::string& docName) {
    std::ifstream file(filePath);
    std::string word;
    int position = 0;

    while (file >> word) {
        position++;
        // Check if the word is in the hash map
        if (hashMap.find(word) == hashMap.end()) {
            // If not, create a new entry
            hashMap[word] = { WordInDocument(docName) };
            hashMap[word][0].addPosition(position);
        }
        else {
            auto& docList = hashMap[word];
            auto it = std::find_if(docList.begin(), docList.end(),
                [&](const WordInDocument& doc) { return doc.documentName == docName; });

            if (it != docList.end()) {
                // If the document already exists, update its positions
                it->addPosition(position);
            }
            else {
                // Otherwise, create a new WordInDocument entry for this document
                docList.emplace_back(docName);
                docList.back().addPosition(position);
            }
        }

        // Trie insertion remains unchanged
        trie.insert(word, docName, position);
    }
    file.close();
}

void SearchEngine::crawlDocuments2(const std::string& folderPath) {
    time_t start, end;
    time(&start);
    for (int i = 1; i < 1000; ++i) {
        std::string filePath = folderPath + "/review_" + std::to_string(i) + ".txt";
        std::ifstream file(filePath);
        if (!file.is_open()) break;

        parseFile(filePath, "review_" + std::to_string(i));
    }
    time(&end);
    std::cout << "Time taken to crawl documents: " << difftime(end, start) << " seconds\n";
}

std::vector<WordInDocument> SearchEngine::searchWithHashMap2(const std::string& query) {
    if (hashMap.find(query) != hashMap.end()) {
        return hashMap[query];
    }
    return {};
}

std::vector<WordInDocument> SearchEngine::searchWithTrie2(const std::string& query) {
    return trie.search(query);
}

void SearchEngine::addNewDocument2(const std::string& filePath) {
    parseFile(filePath, filePath);
}

void SearchEngine::dumpSearchEngine2(const std::string& filePath) {
    std::ofstream file(filePath);
    for (const auto& pair : hashMap) {
        file << pair.first << " ";
        for (const auto& doc : pair.second) {
            file << doc.documentName << " ";
            for (int pos : doc.positions) {
                file << pos << " ";
            }
        }
        file << "\n";
    }
}

void SearchEngine::loadSearchEngine2(const std::string& filePath) {
    std::ifstream rdr(filePath);
    if (!rdr.is_open()) {
        std::cerr << "Error: Unable to open file for loading data." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(rdr, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        std::vector<WordInDocument> docs;
        std::string docName;
        while (iss >> docName) {
            WordInDocument doc(docName);
            int pos;
            while (iss >> pos) {
                doc.addPosition(pos);
                if (iss.peek() == ' ' || iss.peek() == EOF) break;
            }
            docs.push_back(doc);
        }
        hashMap[word] = docs;
    }

    rdr.close();
    std::cout << "Search engine data loaded successfully from " << filePath << "." << std::endl;
}

void SearchEngine::print_loaded_searchengine2(SearchEngine& engine) {
    for (const auto& pair : engine.hashMap) {
        std::cout << pair.first << " ";
        for (const auto& doc : pair.second) {
            std::cout << doc.documentName << " ";
            for (int pos : doc.positions) {
                std::cout << pos << " ";
            }
            std::cout << "(Count: " << doc.positions.size() << ") ";
        }
        std::cout << "\n";
    }
}

void SearchEngine::Search_loaded_engine_hashmap2(SearchEngine& engine, const std::string& query) {
    if (engine.hashMap.find(query) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[query]) {
            std::cout << doc.documentName << " ";
            for (int pos : doc.positions) {
                std::cout << pos << " ";
            }
            std::cout << "(Count: " << doc.positions.size() << ") ";
            std::cout << "\n";
        }
    }
}
*/
void SearchEngine::sort_engine_hashmap(SearchEngine& engine) {
	for (auto& pair : engine.hashMap) {
		std::sort(pair.second.begin(), pair.second.end(),
			[](const WordInDocument& a, const WordInDocument& b) {
				return a.positions.size() > b.positions.size();
			});
	}
}


/////////////////////////////////////////////////

void SearchEngine::crawlDocumentsParallel(const std::string& folderPath, size_t totalFiles, size_t numThreads) {
    time_t start, end;
    time(&start);
    // Prepare the list of file paths
    std::vector<std::string> filePaths;
    for (size_t i = 1; i <= totalFiles; ++i) {
        filePaths.push_back(folderPath + "/review_" + std::to_string(i) + ".txt");
    }

    // Divide file paths into batches for each thread
    size_t filesPerThread = totalFiles / numThreads;
    std::vector<std::thread> threads;

    for (size_t t = 0; t < numThreads; ++t) {
        size_t start = t * filesPerThread;
        size_t end = (t == numThreads - 1) ? totalFiles : start + filesPerThread;
        std::vector<std::string> fileBatch(filePaths.begin() + start, filePaths.begin() + end);

        // Use a lambda function to capture `this` and pass the batch
        threads.emplace_back([this, fileBatch]() {
            this->processFileBatch(fileBatch);
            });
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }
    time(&end);
    std::cout << "Time taken to crawl documents: " << difftime(end, start) << " seconds\n";
}

void SearchEngine::parseFileBuffered(const std::string& filePath, const std::string& docName,
    std::unordered_map<std::string, std::vector<WordInDocument>>& localMap) {
    const size_t bufferSize = 16 * 1024; // 16 KB buffer size
    char buffer[bufferSize];

    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::string currentWord;
    int position = 0;

    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        size_t bytesRead = file.gcount();
        for (size_t i = 0; i < bytesRead; ++i) {
            char c = buffer[i];

            if (std::isspace(static_cast<unsigned char>(c)) || std::ispunct(static_cast<unsigned char>(c))) {
                if (!currentWord.empty()) {
                    position++;

                    if (localMap.find(currentWord) == localMap.end()) {
                        localMap[currentWord] = { WordInDocument(docName) };
                        localMap[currentWord][0].addPosition(position);
                    }
                    else {
                        auto& docs = localMap[currentWord];
                        if (docs.empty() || docs.back().documentName != docName) {
                            docs.emplace_back(docName);
                        }
                        docs.back().addPosition(position);
                    }

                    currentWord.clear();
                }
            }
            else {
                currentWord += c;
            }
        }
    }

    // Process any remaining word
    if (!currentWord.empty()) {
        position++;
        if (localMap.find(currentWord) == localMap.end()) {
            localMap[currentWord] = { WordInDocument(docName) };
            localMap[currentWord][0].addPosition(position);
        }
        else {
            auto& docs = localMap[currentWord];
            if (docs.empty() || docs.back().documentName != docName) {
                docs.emplace_back(docName);
            }
            docs.back().addPosition(position);
        }
    }

    file.close();
}
//
void SearchEngine::processFileBatch(const std::vector<std::string>& fileBatch) {
    for (const auto& filePath : fileBatch) {
        std::string docName = filePath.substr(filePath.find_last_of("/") + 1); // Extract document name

        // Local map to hold word-document-position mappings for this batch
        std::unordered_map<std::string, std::vector<WordInDocument>> localMap;

        // Parse file into a local map to reduce locking
        parseFileBuffered(filePath, docName, localMap);

        // Merge local map into global hashMap under lock
        {
            std::lock_guard<std::mutex> guard(mapMutex); // Synchronize access to hashMap
            for (const auto& pair : localMap) {
                const std::string& word = pair.first;
                const std::vector<WordInDocument>& docs = pair.second;

                auto& globalDocs = hashMap[word]; // Directly get or create the global entry

                for (const auto& doc : docs) {
                    auto it = std::find_if(globalDocs.begin(), globalDocs.end(),
                        [&](const WordInDocument& d) { return d.documentName == doc.documentName; });
                    if (it != globalDocs.end()) {
                        // Merge positions into existing document entry
                        it->positions.insert(it->positions.end(), doc.positions.begin(), doc.positions.end());
                    }
                    else {
                        // Add new document entry
                        globalDocs.push_back(doc); // Use move to avoid copying
                    }
                }
            }
        }
    }
}
////////////////////////////////////////////
//b.
//Two Words : WORD1 + WORD2
//We should get all the documents which should contain either of the two words, sorted
//by those which has both the words and then the priority is which contains the highest
//occurrences of either of the word)
//input will vbe like dog+Dog now i will have to return justy like simpole search  with hashmap whiole al;so keeping above three lines in mind too
/*
void SearchEngine::Search_loaded_engine_hashmap_2_words(SearchEngine& engine, const std::string& query) {
    std::string word1, word2;
    std::istringstream iss(query);
    std::getline(iss, word1, '+');
    std::getline(iss, word2);

    std::vector<WordInDocument> results1, results2;
    if (engine.hashMap.find(word1) != engine.hashMap.end()) {
        results1 = engine.hashMap[word1];
    }
    if (engine.hashMap.find(word2) != engine.hashMap.end()) {
        results2 = engine.hashMap[word2];
    }

    std::vector<WordInDocument> results;
    std::set<std::string> docNames;
#include <set>
    for (const auto& doc : results1) {
        docNames.insert(doc.documentName);
    }
    for (const auto& doc : results2) {
        docNames.insert(doc.documentName);
    }

    for (const auto& docName : docNames) {
        bool found1 = false, found2 = false;
        int count1 = 0, count2 = 0;
        for (const auto& doc : results1) {
            if (doc.documentName == docName) {
                found1 = true;
                count1 = doc.positions.size();
                break;
            }
        }
        for (const auto& doc : results2) {
            if (doc.documentName == docName) {
                found2 = true;
                count2 = doc.positions.size();
                break;
            }
        }

        if (found1 || found2) {
            results.push_back(WordInDocument(docName));
            results.back().addPosition(count1 + count2);
        }
    }

    std::sort(results.begin(), results.end(),
        [](const WordInDocument& a, const WordInDocument& b) {
            return a.positions.size() > b.positions.size();
        });

    for (const auto& doc : results) {
        std::cout << doc.documentName << " ";
        for (int pos : doc.positions) {
            std::cout << pos << " ";
        }
        std::cout << "\n";
    }
}
*/


///////////
void SearchEngine::Search_loaded_engine_hashmap_words_2(SearchEngine& engine, const std::string& query1, const std::string& query2) {
    std::unordered_map<std::string, int> documentPriority; // Document -> Total occurrences score
    std::unordered_map<std::string, int> bothWordsCount;   // Document -> Mark if contains both words
    std::unordered_map<std::string, std::vector<int>> combinedPositions; // Document -> Combined positions of occurrences

    // Collect occurrences for query1
    if (engine.hashMap.find(query1) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[query1]) {
            documentPriority[doc.documentName] += doc.positions.size(); // Add occurrences of query1
            combinedPositions[doc.documentName].insert(
                combinedPositions[doc.documentName].end(),
                doc.positions.begin(),
                doc.positions.end()
            );
        }
    }

    // Collect occurrences for query2
    if (engine.hashMap.find(query2) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[query2]) {
            documentPriority[doc.documentName] += doc.positions.size(); // Add occurrences of query2
            combinedPositions[doc.documentName].insert(
                combinedPositions[doc.documentName].end(),
                doc.positions.begin(),
                doc.positions.end()
            );
        }
    }

    // Mark documents that contain both words
    for (const auto& doc : combinedPositions) {
        bool containsQuery1 = engine.hashMap[query1].end() != std::find_if(
            engine.hashMap[query1].begin(), engine.hashMap[query1].end(),
            [&](const WordInDocument& entry) { return entry.documentName == doc.first; });
        bool containsQuery2 = engine.hashMap[query2].end() != std::find_if(
            engine.hashMap[query2].begin(), engine.hashMap[query2].end(),
            [&](const WordInDocument& entry) { return entry.documentName == doc.first; });
        if (containsQuery1 && containsQuery2) {
            bothWordsCount[doc.first] = 1; // Mark the document as containing both words
        }
    }

    // Sort documents based on priority and presence of both words
    std::vector<std::string> sortedDocuments;
    for (const auto& entry : documentPriority) {
        sortedDocuments.push_back(entry.first);
    }
    std::sort(sortedDocuments.begin(), sortedDocuments.end(), [&](const std::string& a, const std::string& b) {
        if (bothWordsCount[a] != bothWordsCount[b]) {
            return bothWordsCount[a] > bothWordsCount[b]; // Documents with both words come first
        }
        return documentPriority[a] > documentPriority[b]; // Sort by total occurrences
        });

    // Output results with context
    for (const auto& docName : sortedDocuments) {
        std::cout << docName << " ";
        for (int pos : combinedPositions[docName]) {
            std::cout << pos << " ";
            std::ifstream file("review_text/" + docName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + 5) {
                        break;
                    }
                }

                // Print the context words
                for (const auto& w : words) {
                    std::cout << w << " ";
                }
                std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << docName << std::endl;
            }
        }
        std::cout << "(Count: " << documentPriority[docName] << ") ";
        std::cout << "\n";
    }
}
////////////////////////////////////////////////////////

void SearchEngine::Search_loaded_engine_hashmap_words_multiple(SearchEngine& engine, const std::string& query) {
    // Parse the words from the query, ignoring the last word
    std::istringstream stream(query);
    std::vector<std::string> searchWords;
    std::string word;
    while (stream >> word) {
        if (word != ".") {
            searchWords.push_back(word);
        }
    }

    std::unordered_map<std::string, int> documentPriority; // Document -> Total occurrences score
    std::unordered_map<std::string, int> wordMatchCount;   // Document -> Count of matched search words
    std::unordered_map<std::string, std::vector<int>> combinedPositions; // Document -> Combined positions of occurrences

    // Process each search word
    for (const auto& searchWord : searchWords) {
        if (engine.hashMap.find(searchWord) != engine.hashMap.end()) {
            for (const auto& doc : engine.hashMap[searchWord]) {
                documentPriority[doc.documentName] += doc.positions.size();
                wordMatchCount[doc.documentName]++; // Count how many search words this document contains
                combinedPositions[doc.documentName].insert(
                    combinedPositions[doc.documentName].end(),
                    doc.positions.begin(),
                    doc.positions.end()
                );
            }
        }
    }

    // Sort documents
    std::vector<std::string> sortedDocuments;
    for (const auto& entry : documentPriority) {
        sortedDocuments.push_back(entry.first);
    }
    std::sort(sortedDocuments.begin(), sortedDocuments.end(), [&](const std::string& a, const std::string& b) {
        if (wordMatchCount[a] != wordMatchCount[b]) {
            return wordMatchCount[a] > wordMatchCount[b]; // Documents with more matched words come first
        }
        return documentPriority[a] > documentPriority[b]; // Sort by total occurrences
        });

    // Output results with context
    for (const auto& docName : sortedDocuments) {
        std::cout << docName << " ";
        for (int pos : combinedPositions[docName]) {
            std::cout << pos << " ";
            std::ifstream file("review_text/" + docName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + 5) {
                        break;
                    }
                }

                // Print context words
                for (const auto& w : words) {
                    std::cout << w << " ";
                }
                std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << docName << std::endl;
            }
        }
        std::cout << "(Count: " << documentPriority[docName] << ", Words Matched: " << wordMatchCount[docName] << ") ";
        std::cout << "\n";
    }
}

/////////////////////////////////
void SearchEngine::Search_loaded_engine_hashmap_subtraction(SearchEngine& engine, const std::string& query1, const std::string& query2) {
    std::unordered_set<std::string> w1Documents; // Set of documents containing W1
    std::unordered_set<std::string> w2Documents; // Set of documents containing W2
    std::unordered_map<std::string, std::vector<int>> w1Positions; // W1 document positions

    // Retrieve documents for W1
    if (engine.hashMap.find(query1) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[query1]) {
            w1Documents.insert(doc.documentName);
            w1Positions[doc.documentName] = doc.positions;
        }
    }

    // Retrieve documents for W2
    if (engine.hashMap.find(query2) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[query2]) {
            w2Documents.insert(doc.documentName);
        }
    }

    // Perform subtraction: W1 - W2
    std::vector<std::string> resultDocuments;
    for (const auto& docName : w1Documents) {
        if (w2Documents.find(docName) == w2Documents.end()) {
            resultDocuments.push_back(docName);
        }
    }

    // Output results with context
    for (const auto& docName : resultDocuments) {
        std::cout << docName << " ";
        for (int pos : w1Positions[docName]) {
            std::cout << pos << " ";
            std::ifstream file("review_text/" + docName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + 5) {
                        break;
                    }
                }

                // Print context words
                for (const auto& w : words) {
                    std::cout << w << " ";
                }
                std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << docName << std::endl;
            }
        }
        std::cout << "(Count: " << w1Positions[docName].size() << ") ";
        std::cout << "\n";
    }
}

/////////
void SearchEngine::Search_loaded_engine_hashmap_sentence(SearchEngine& engine, const std::string& query) {
    // Extract words from the query
    if (query.front() != '\"' || query.back() != '\"') {
        std::cerr << "Error: Sentence search query must be enclosed in quotation marks.\n";
        return;
    }

    std::string sentence = query.substr(1, query.size() - 2); // Remove quotation marks
    std::istringstream stream(sentence);
    std::vector<std::string> words;
    std::string word;
    while (stream >> word) {
        words.push_back(word);
    }

    if (words.empty()) {
        std::cerr << "Error: No words found in the sentence query.\n";
        return;
    }

    std::unordered_map<std::string, int> sentenceOccurrences; // Document -> Count of sentence occurrences
    std::unordered_map<std::string, std::vector<int>> sentencePositions; // Document -> Sentence start positions

    // Check each document for the sentence
    if (engine.hashMap.find(words[0]) != engine.hashMap.end()) {
        for (const auto& doc : engine.hashMap[words[0]]) {
            const std::string& docName = doc.documentName;
            const std::vector<int>& positions = doc.positions;

            std::ifstream file("review_text/" + docName + ".txt");
            if (!file.is_open()) {
                std::cerr << "Error: Could not open file " << docName << std::endl;
                continue;
            }

            // Read the document into a vector of words
            std::vector<std::string> documentWords;
            while (file >> word) {
                documentWords.push_back(word);
            }
            file.close();

            // Check for the sentence in consecutive positions
            for (int pos : positions) {
                bool sentenceFound = true;
                for (size_t i = 1; i < words.size(); ++i) {
                    if (pos + i >= documentWords.size() || documentWords[pos + i - 1] != words[i]) {
                        sentenceFound = false;
                        break;
                    }
                }
                if (sentenceFound) {
                    sentenceOccurrences[docName]++;
                    sentencePositions[docName].push_back(pos);
                }
            }
        }
    }

    // Sort documents by the number of occurrences
    std::vector<std::pair<std::string, int>> sortedDocuments(sentenceOccurrences.begin(), sentenceOccurrences.end());
    std::sort(sortedDocuments.begin(), sortedDocuments.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second > b.second; // Descending order
    });

    // Output results
    for (const auto& doc : sortedDocuments) {
        const std::string& docName = doc.first;
        int count = doc.second;
        std::cout << docName << " (Occurrences: " << count << ")\n";
        for (int pos : sentencePositions[docName]) {
            std::cout << "Position: " << pos << " ";
            std::ifstream file("review_text/" + docName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + words.size() + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + words.size() + 5) {
                        break;
                    }
                }

                // Print context words
                for (const auto& w : words) {
                    std::cout << w << " ";
                }
                std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << docName << std::endl;
            }
        }
        std::cout << "\n";
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SearchEngine::crawlDocuments_trie(const std::string& folderPath) {
	time_t start, end;
	time(&start);
	for (int i = 1; i < 20000; ++i) {
		std::string filePath = folderPath + "/review_" + std::to_string(i) + ".txt";
		std::ifstream file(filePath);
		if (!file.is_open()) break;

		parseFile_trie(filePath, "review_" + std::to_string(i));
	}
	time(&end);
	std::cout << "Time taken to crawl documents: " << difftime(end, start) << " seconds\n";

}
void SearchEngine::parseFile_trie(const std::string& filePath, const std::string& docName){
	std::ifstream file(filePath);
	std::string word;
	int position = 0;

	while (file >> word) {
		position++;
		trie.insert(word, docName, position);
	}
	file.close();

}
void SearchEngine::trie_dump(const std::string& filePath) {
	//std::ofstream file(filePath);
	trie.dump(filePath);
	//file.close();
	std::cout << "Trie data dumped successfully to " << filePath << "." << std::endl;

}
void SearchEngine::trie_load(const std::string& filePath) {
	//std::ifstream file(filePath);
	trie.load(filePath);
	//file.close();
	std::cout << "Trie data loaded successfully from " << filePath << "." << std::endl;
}
void SearchEngine::search_trie_print(const std::string& folderPath, std::string query) {
    std::vector<WordInDocument> results;
    results = trie.search(query);
    //sort results
    std::sort(results.begin(), results.end(),
        [](const WordInDocument& a, const WordInDocument& b) {
            return a.positions.size() > b.positions.size();
        });
    //
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (const auto& doc : results) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << doc.documentName << " \n";
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        for (int pos : doc.positions) {
            std::cout << "\n" << pos << " ";
            std::ifstream file(folderPath + "/" + doc.documentName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + 5) {
                        break;
                    }
                }

                // Print the words
                int cc = 0;
                for (const auto& w : words) {
                    if (cc == 5) {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                        std::cout << w ;
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        std::cout << " ";
                    } else {
                        std::cout << w << " ";
                    }
                    cc++;
                }
                std::cout << "\n";
                file.close();
            } else {
                std::cerr << "Error: Could not open file " << doc.documentName << std::endl;
            }
        }
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "\n" << "(Count: " << doc.positions.size() << ") ";
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "\n\n";
    }
}
//////////

void SearchEngine::search_trie_print_2_words(const std::string& folderPath, std::string query) {
    std::string word1, word2;
    size_t delimiterPos = query.find('+');
    if (delimiterPos == std::string::npos) {
        std::cerr << "Error: Query must contain two words separated by '+'\n";
        return;
    }
    word1 = query.substr(0, delimiterPos);
    word2 = query.substr(delimiterPos + 1);

    std::vector<WordInDocument> results1 = trie.search(word1);
    std::vector<WordInDocument> results2 = trie.search(word2);

    std::unordered_map<std::string, std::pair<int, bool>> documentScores; // Document -> (Total occurrences, Contains both words)
    std::unordered_map<std::string, std::vector<int>> combinedPositions;

    // Process occurrences for word1
    for (const auto& doc : results1) {
        documentScores[doc.documentName].first += doc.positions.size();
        documentScores[doc.documentName].second = false;
        combinedPositions[doc.documentName].insert(
            combinedPositions[doc.documentName].end(),
            doc.positions.begin(),
            doc.positions.end());
    }

    // Process occurrences for word2
    for (const auto& doc : results2) {
        documentScores[doc.documentName].first += doc.positions.size();
        if (documentScores.find(doc.documentName) != documentScores.end()) {
            documentScores[doc.documentName].second = true;
        }
        combinedPositions[doc.documentName].insert(
            combinedPositions[doc.documentName].end(),
            doc.positions.begin(),
            doc.positions.end());
    }

    // Sort documents
    std::vector<std::string> sortedDocuments;
    for (const auto& entry : documentScores) {
        sortedDocuments.push_back(entry.first);
    }
    std::sort(sortedDocuments.begin(), sortedDocuments.end(), [&](const std::string& a, const std::string& b) {
        if (documentScores[a].second != documentScores[b].second) {
            return documentScores[a].second > documentScores[b].second; // Documents with both words come first
        }
        return documentScores[a].first > documentScores[b].first; // Sort by total occurrences
        });
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Output results
    for (const auto& docName : sortedDocuments) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << docName << " ";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        for (int pos : combinedPositions[docName]) {
            std::cout << "\n" << pos << " ";
            std::ifstream file(folderPath + "/" + docName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + 5) {
                        break;
                    }
                }

                // Print context words
                int cc = 0;
                for (const auto& w : words) {
                    if (cc == 5) {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                        std::cout << w;
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        std::cout << " ";
					}
					else {
						std::cout << w << " ";
					}
                    cc++;
                }
                std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << docName << std::endl;
            }
        }
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "\n" << "(Count: " << documentScores[docName].first << ") ";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "\n";
    }
}
////////////////////////////////////////////////
void SearchEngine::search_trie_print_multiple(const std::string& folderPath, std::string query) {
    std::istringstream stream(query);
    std::vector<std::string> words;
    std::string word;
    while (stream >> word) {
        if (word != ".") {
            words.push_back(word);
        }
    }

    std::unordered_map<std::string, std::pair<int, int>> documentScores; // Document -> (Total occurrences, Matched words count)
    std::unordered_map<std::string, std::vector<int>> combinedPositions;

    // Process occurrences for each word
    for (const auto& searchWord : words) {
        std::vector<WordInDocument> results = trie.search(searchWord);
        for (const auto& doc : results) {
            documentScores[doc.documentName].first += doc.positions.size();
            documentScores[doc.documentName].second++;
            combinedPositions[doc.documentName].insert(
                combinedPositions[doc.documentName].end(),
                doc.positions.begin(),
                doc.positions.end());
        }
    }

    // Sort documents
    std::vector<std::string> sortedDocuments;
    for (const auto& entry : documentScores) {
        sortedDocuments.push_back(entry.first);
    }
    std::sort(sortedDocuments.begin(), sortedDocuments.end(), [&](const std::string& a, const std::string& b) {
        if (documentScores[a].second != documentScores[b].second) {
            return documentScores[a].second > documentScores[b].second; // Documents with more matched words come first
        }
        return documentScores[a].first > documentScores[b].first; // Sort by total occurrences
        });

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Output results
    for (const auto& docName : sortedDocuments) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "\n" << docName << " ";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        for (int pos : combinedPositions[docName]) {
            std::cout << "\n" << pos << " ";
            std::ifstream file(folderPath + "/" + docName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + 5) {
                        break;
                    }
                }
                int cc = 0;
                // Print context words
                for (const auto& w : words) {
                    //std::cout << w << " ";
                    if (cc == 5) {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
						std::cout << w ;
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
						std::cout << " ";
					}
					else {
						std::cout << w << " ";
                    }
                    cc++;
                }
				std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << docName << std::endl;
            }
        }
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "(Count: " << documentScores[docName].first << ", Words Matched: " << documentScores[docName].second << ") ";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "\n";
    }
}
/////////////////////////
/*
void SearchEngine::search_trie_subtraction(const std::string& folderPath, std::string query) {
    size_t delimiterPos = query.find('-');
    if (delimiterPos == std::string::npos) {
        std::cerr << "Error: Query must be in the format W1-W2\n";
        return;
    }
    std::string word1 = query.substr(0, delimiterPos);
    std::string word2 = query.substr(delimiterPos + 1);

    std::vector<WordInDocument> results1 = trie.search(word1);
    std::vector<WordInDocument> results2 = trie.search(word2);

    std::unordered_set<std::string> excludedDocuments;
    for (const auto& doc : results2) {
        excludedDocuments.insert(doc.documentName);
    }

    // Output documents with word1 but not word2
    for (const auto& doc : results1) {
        if (excludedDocuments.find(doc.documentName) == excludedDocuments.end()) {
            std::cout << doc.documentName << " ";
            for (int pos : doc.positions) {
                std::cout << pos << " ";
                std::ifstream file(folderPath + "/" + doc.documentName + ".txt");
                if (file.is_open()) {
                    std::string word;
                    std::vector<std::string> words;
                    int currentPos = 0;

                    while (file >> word) {
                        currentPos++;
                        if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                            words.push_back(word);
                        }
                        if (currentPos > pos + 5) {
                            break;
                        }
                    }

                    // Print context words
                    for (const auto& w : words) {
                        std::cout << w << " ";
                    }
                    file.close();
                }
                else {
                    std::cerr << "Error: Could not open file " << doc.documentName << std::endl;
                }
            }
            std::cout << "(Count: " << doc.positions.size() << ")\n";
        }
    }
}
*/
void SearchEngine::search_trie_subtraction(const std::string& folderPath, std::string query) {
    size_t delimiterPos = query.find('-');
    if (delimiterPos == std::string::npos) {
        std::cerr << "Error: Query must be in the format W1-W2\n";
        return;
    }
    std::string word1 = query.substr(0, delimiterPos);
    std::string word2 = query.substr(delimiterPos + 1);

    std::vector<WordInDocument> results1 = trie.search(word1);
    std::vector<WordInDocument> results2 = trie.search(word2);

    std::unordered_set<std::string> excludedDocuments;
    for (const auto& doc : results2) {
        excludedDocuments.insert(doc.documentName);
    }

    // Filter results to include only documents with word1 but not word2
    std::vector<WordInDocument> filteredResults;
    for (const auto& doc : results1) {
        if (excludedDocuments.find(doc.documentName) == excludedDocuments.end()) {
            filteredResults.push_back(doc);
        }
    }

    // Sort the results by occurrence count in descending order
    std::sort(filteredResults.begin(), filteredResults.end(), [](const WordInDocument& a, const WordInDocument& b) {
        return a.positions.size() > b.positions.size();
        });
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Output the sorted results
    for (const auto& doc : filteredResults) {
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << doc.documentName << " ";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        for (int pos : doc.positions) {
            std::cout << "\n" << pos << " ";
            std::ifstream file(folderPath + "/" + doc.documentName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + 5) {
                        break;
                    }
                }

                // Print context words
                int cc = 0;
                for (const auto& w : words) {
                    //std::cout << w << " ";
                    if (cc == 5) {
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
						std::cout << w;
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
						std::cout << " ";
                    }
					else {
						std::cout << w << " ";
					}
					cc++;
                }
				std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << doc.documentName << std::endl;
            }
        }
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << " " << "(Count: " << doc.positions.size() << ")\n";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}
//////////////////////////////////////////////////////////
void SearchEngine::search_trie_sentence(const std::string& folderPath, std::string query) {
    if (query.front() != '"' || query.back() != '"') {
        std::cerr << "Error: Sentence query must be enclosed in quotation marks.\n";
        return;
    }

    // Remove quotation marks from the query
    query = query.substr(1, query.size() - 2);
    std::istringstream stream(query);
    std::vector<std::string> sentenceWords;
    std::string word;
    while (stream >> word) {
        sentenceWords.push_back(word);
    }

    if (sentenceWords.empty()) {
        std::cerr << "Error: No words found in the sentence query.\n";
        return;
    }

    std::unordered_map<std::string, int> sentenceOccurrences; // Document -> Count of sentence occurrences
    std::unordered_map<std::string, std::vector<int>> sentencePositions; // Document -> Sentence start positions

    // Check each document for the sentence
    std::vector<WordInDocument> firstWordResults = trie.search(sentenceWords[0]);
    for (const auto& doc : firstWordResults) {
        const std::string& docName = doc.documentName;
        const std::vector<int>& positions = doc.positions;

        std::ifstream file(folderPath + "/" + docName + ".txt");
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << docName << std::endl;
            continue;
        }

        // Read the document into a vector of words
        std::vector<std::string> documentWords;
        while (file >> word) {
            documentWords.push_back(word);
        }
        file.close();

        // Check for the sentence in consecutive positions
        for (int pos : positions) {
            bool sentenceFound = true;
            for (size_t i = 1; i < sentenceWords.size(); ++i) {
                if (pos + i - 1 >= documentWords.size() || documentWords[pos + i - 1] != sentenceWords[i]) {
                    sentenceFound = false;
                    break;
                }
            }
            if (sentenceFound) {
                sentenceOccurrences[docName]++;
                sentencePositions[docName].push_back(pos);
            }
        }
    }

    // Sort documents by the number of sentence occurrences
    std::vector<std::pair<std::string, int>> sortedDocuments(sentenceOccurrences.begin(), sentenceOccurrences.end());
    std::sort(sortedDocuments.begin(), sortedDocuments.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second > b.second; // Descending order
        });

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Output results
    for (const auto& doc : sortedDocuments) {
        const std::string& docName = doc.first;
        int count = doc.second;
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << docName;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << " (Occurrences: " << count << ")\n";
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        for (int pos : sentencePositions[docName]) {
            std::cout << "\n" << "Position: " << pos << " ";
            std::ifstream file(folderPath + "/" + docName + ".txt");
            if (file.is_open()) {
                std::string word;
                std::vector<std::string> words;
                int currentPos = 0;

                while (file >> word) {
                    currentPos++;
                    if (currentPos >= pos - 5 && currentPos <= pos + sentenceWords.size() + 5) {
                        words.push_back(word);
                    }
                    if (currentPos > pos + sentenceWords.size() + 5) {
                        break;
                    }
                }

                // Print context words
                int cc = 0;
                for (const auto& w : words) {
					if (cc >= 5 && cc<= (5+sentenceWords.size()-1)) {
                        int rr = 0;
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
						std::cout << w;
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
						std::cout << " ";
					}
					else {
						std::cout << w << " ";
					}
                    //std::cout << w << " ";
                    cc++;
                }
                std::cout << "\n";
                file.close();
            }
            else {
                std::cerr << "Error: Could not open file " << docName << std::endl;
            }
        }
        std::cout << "\n";
    }
}
/////////////////////////////////////////
/*
void SearchEngine::addNewDocument_trie() {
	std::string docName;
	std::cout << "Enter the document name: ";
	std::cin >> docName;

	std::string filePath = "review_text/" + docName + ".txt";
	std::ofstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Error: Could not create file " << filePath << std::endl;
		return;
	}

	std::string text;
	std::cout << "Enter the document text (type 'end' on a new line to finish):\n";
	while (true) {
		std::getline(std::cin, text);
		if (text == "end") {
			break;
		}
		file << text << "\n";
	}
	file.close();

	parseFile_trie(filePath, docName);
	std::cout << "Document added successfully.\n";
}
*/
///////////////////////////////////////////////
