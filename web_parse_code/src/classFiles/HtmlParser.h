#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class HtmlParser
{
    
    public:
    enum RefineMethod { 
    REF_LARGEST = 0,
    REF_SMALLEST,
    REF_ALL,
    };
    HtmlParser() {}
    HtmlParser(const std::string& file): htmlFileName(file) {}
    HtmlParser(const std::vector<std::string>& toCopy): htmlStrings(toCopy) {}
    HtmlParser(const HtmlParser& toCopy) {
        this->htmlFileName = toCopy.htmlFileName;
        this->htmlStrings = toCopy.htmlStrings;
    }


    void readHTMLFile(const std::string& filename);
    void readHTMLFile(void) { readHTMLFile(htmlFileName); }
    bool refineSearch(const std::string& strToFind);
    bool refineSearch(const std::string& strToFind, const RefineMethod& method);
    bool refineSearch(const std::string& strToFind, const std::string& tag);
    bool extractElement(const std::string& strToFind, HtmlParser& extractTo);
    bool empty(void) const { return htmlStrings.empty(); }
    bool has(const std::string& str) const;
    void printHTML() const;
    void manualPrint(const int& lines) const;
    bool searchInElement(const std::string& strToFind, std::vector<std::string>& foundStrings) const;
    bool searchInElement(std::vector<std::string>& foundStrings) const;
    std::vector<std::string> searchInElement(const std::string& strToFind) const;


    private:
    std::string htmlFileName;
    std::vector<std::string> htmlStrings;

    void findHTMLIterators(int& itStart, int& itEnd, const std::string& strToFind) const;
    void findHTMLIterators(int& itStart, int& itEnd, const std::string& strToFind, const std::string& tagToEnd) const;
    bool findSingleString(std::string& htmlLine, const std::string& strToFind, std::string& foundString) const;

    void deleteHtmlTags(std::string &htmlLine) const;


};

void printComponents(const std::vector<std::vector<std::string>>& componentVector, const std::string& componentName);


#endif
