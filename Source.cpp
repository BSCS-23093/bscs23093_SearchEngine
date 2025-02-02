#include "SearchEngine.h"
#include <iostream>


int main() {
    SearchEngine engine;
    std::string folderPath = "review_text";
	//engine.addNewDocument_trie(folderPath);
    // Crawl documents
    engine.crawlDocuments(folderPath);
    engine.crawlDocuments_trie(folderPath);
	//engine.trie_dump("search_engine_trie.dat");
	//engine.trie_load("search_engine_trie.dat");
	//engine.search_trie_print(folderPath);
    //engine.searchWithTrie(folderPath);
    //
	//std::string query;
	//std::cout << "Enter your query:(for twords w1+w2) ";
	//std::getline(std::cin, query);
    //std::string query = "\"dog foods\"";
	//engine.search_trie_sentence(folderPath, query);
	//engine.search_trie_subtraction(folderPath, query);
	//engine.search_trie_print_2_words(folderPath, query);
    //engine.search_trie_print_multiple(folderPath, query);
    //engine.search_trie_print(folderPath,query);
	//engine.searchWithTrie(query);

    // 
	//engine.crawlDocumentsParallel(folderPath,20000,14);
	//engine.dumpSearchEngine("tester.dat");
	//engine.loadSearchEngine("search_engine.dat");
	//engine.Search_hashmap_1word(engine,"dog");
	

	//engine.sort_engine_hashmap(engine);
	//engine.print_loaded_searchengine(engine);
    // Query examples
    //std::string query;
    //std::cout << "Enter your query: ";
    //std::getline(std::cin, query);

   // std::string query2;
    //std::cout << "Enter your query: ";
    //std::getline(std::cin, query2);

    //std::string query = "\"dog foods\"";
    //engine.Search_loaded_engine_hashmap_sentence(engine, query);

	//engine.Search_loaded_engine_hashmap_subtraction(engine, query, query2);
	//engine.Search_loaded_engine_hashmap_words_multiple(engine, query);

	//engine.Search_loaded_engine_hashmap_words_2(engine,query,query2);
    /*
    auto results = engine.searchWithHashMap(query);
    std::cout << "Results from HashMap:\n";
    for (const auto& res : results) {
        std::cout << res.documentName << " | Occurrences: " << res.positions.size() << " | Positions: ";
        for (int pos : res.positions) {
            std::cout << pos << " ";
        }
        std::cout << "\n";
    }
    */
    /*
    auto results2 = engine.searchWithTrie(query);
    std::cout << "Results from Trie:\n";
    for (const auto& res2 : results2) {
        std::cout << res2.documentName << " (" << res2.positions.size() << " times)\n";
    }
    */

    return 0;
}