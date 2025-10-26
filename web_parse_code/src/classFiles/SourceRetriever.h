#ifndef SOURCE_RETRIEVER_H
#define SOURCE_RETRIEVER_H

#include <iostream>
#include <vector>
#include <fstream>
#include "HtmlParser.h"
#include "WikiScalper.h"

class PartByCredibility
{
    public:
    PartByCredibility(const std::vector<std::string>& d, const int& i, const bool& r): linkInd(i), credibleDatabases(d), reverse(r) {}
    bool operator() (const std::vector<std::vector<std::string>>& element) {
        const std::vector<std::string>& linkVec = element[linkInd];
        for(const std::string& link: linkVec) {
            for(const std::string& database: credibleDatabases) {
                if(link.find(database) != std::string::npos) { return (reverse != true); }
            }
        }
        return (reverse != false);
    }
    private:
    const int linkInd;
    const std::vector<std::string> credibleDatabases;
    const bool reverse;
};

class PartByConsistentChar
{
    public:
    PartByConsistentChar(const char& c, const int& ci, const int& i, const bool& r): linkInd(i), charInd(ci), firstChar(c), reverse(r) {}
    bool operator() (const std::vector<std::vector<std::string>>& element) {
        const std::vector<std::string>& linkVec = element[linkInd];
        for(const std::string& link: linkVec) {
            int intercept = 0;
            if(link[charInd] == 'a' && link[charInd + 1] == 'b') { intercept = 2; }
            if(link[charInd + intercept] == firstChar) { return (reverse != true); } // normal true
        }
        return (reverse != false); // normal false
    }
    private:
    const int linkInd;
    const int charInd;
    const char firstChar;
    const bool reverse;
};


class SourceData : public std::vector<std::vector<std::vector<std::string>>> {
public:
    
    SourceData() {}
    SourceData(const SourceData& toCopy) = default;
    SourceData(const std::vector<std::vector<std::vector<std::string>>>& toCopy) 
        : std::vector<std::vector<std::vector<std::string>>>(toCopy) {}
    SourceData(std::vector<std::vector<std::vector<std::string>>>::iterator start, 
               std::vector<std::vector<std::vector<std::string>>>::iterator end)
        : std::vector<std::vector<std::vector<std::string>>>(start, end) {}

    // Assignment operators
    SourceData& operator=(const SourceData& rhs) = default;
    SourceData& operator=(const std::vector<std::vector<std::vector<std::string>>>& rhs) {
        this->assign(rhs.begin(), rhs.end());
        return *this;
    }

    // Function to remove a specific character at given indices
    void removeSpecificChar(char charToRemove, int subElementIdx, int strIdx, int charIdx) {
        for (auto& element : *this) {
            if (subElementIdx < element.size() && strIdx < element[subElementIdx].size() &&
                charIdx < element[subElementIdx][strIdx].size() &&
                element[subElementIdx][strIdx][charIdx] == charToRemove) {
                
                element[subElementIdx][strIdx].erase(element[subElementIdx][strIdx].begin() + charIdx);
            }
        }
    }
    
    template <typename Predicate>
    void partition(SourceData& negativeData, Predicate func) {     // Partition by credibility
        SourceData::iterator databasePartIt = std::partition(
            this->begin(), this->end(),
            func // 1 is the index of the links
        );

        // Copy remaining sources over and erase them from credible sources
        negativeData = SourceData(databasePartIt, this->end());
        this->erase(databasePartIt, this->end());
    }

    bool loadSourceData(std::vector<std::pair<std::string, int>>& loadedCitations, const std::string& filename) {
        bool output = false;
        std::ifstream inputFile(filename, std::ios::binary);

        // Check if inputFile is open
        if (inputFile.is_open()) {
            size_t size;
            inputFile.read(reinterpret_cast<char*>(&size), sizeof(size));  // Read the size of the vector

            // Load each pair (citation and label)
            for (size_t i = 0; i < size; ++i) {
                size_t citationLength;
                inputFile.read(reinterpret_cast<char*>(&citationLength), sizeof(citationLength));  // Read the length of the citation
                std::string citation(citationLength, '\0');
                inputFile.read(&citation[0], citationLength);  // Read the citation
                int label;
                inputFile.read(reinterpret_cast<char*>(&label), sizeof(label));  // Read the label
                loadedCitations.push_back({citation, label});
            }

            std::cout << "Data loaded from " + filename << std::endl;

            this->eraseSimilarCitations(loadedCitations);

            output = true;
        }
        else { output = false; }
    
        inputFile.close();
        return output;
    }
    
void eraseSimilarCitations(const std::vector<std::pair<std::string, int>>& pairs) {
    for (int i = this->size() - 1; i >= 0; --i) {
        std::vector<std::vector<std::string>>& element = (*this)[i];
        for (const auto& pair : pairs) {
            if (element[0][0] == pair.first) {
                this->erase(this->begin() + i);
                break; // break to avoid double-erasing the same index
            }
        }
    }
}

};

class SourceRetriever
{
    public:
    enum LinkMethod {
        LINK_DEFAULT = 0,
        LINK_INPUT,
    };
    SourceRetriever(const LinkMethod& m): method(m) { allElements = SourceData(); WikiScalper::htmlCount = 0;  /* Initialize counter */ }
    SourceRetriever() { WikiScalper::htmlCount = 0;  /* Initialize counter */ }

    void scalp(void);
    void parseLinks();
    void parseLinksAndCitations();
    bool parseReferences();
    void setLinkMethod(const LinkMethod& m) { method = m; }
    static void print(const SourceData& componentVector, const std::string& componentName);
    void printElements(void);
    std::vector<HtmlParser> getHtmlRefrences() { return htmlRefrences; }
    SourceData getElements() { return allElements; }


    
    private:
    LinkMethod method;
    std::vector<HtmlParser> htmlRefrences;
    // std::vector<std::vector<std::string>> allLinks;
    SourceData allElements;
    // std::vector<std::string> credibleLinks;
};

#endif