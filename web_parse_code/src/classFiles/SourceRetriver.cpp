#include "SourceRetriever.h"

void SourceRetriever::scalp(void) {
    std::string url;
    switch(method) {
        case SourceRetriever::LINK_DEFAULT: {
            url = "https://en.wikipedia.org/wiki/Biodiversity";// Change this URL to test with different inputs
        } break;
        case SourceRetriever::LINK_INPUT: {
            url = "https://en.wikipedia.org/wiki/";// Change this URL to test with different inputs
            std::string userInput;
            std::cout << "Enter Wikipedia Search:\n";
            std::cin >> userInput;
            url += userInput;
        } break;
        default: {
            url = "https://en.wikipedia.org/wiki/Biodiversity";// Change this URL to test with different inputs
        } break;

    }



    //////* Python Code *////////
    // WikiScalper::htmlCount = 0;
    WikiScalper scalper;
    scalper.setScript("scalper");
    scalper.setURL(url);

    scalper.initialize();
    scalper.runScript();
}

void SourceRetriever::parseLinks() {
    htmlRefrences.clear();
    allElements.clear();

    if(htmlRefrences.empty()) parseReferences();
    // Searches for all the links within each refrence
    std::vector<std::vector<std::string>> elementsToPush;
    std::vector<std::string> linksToPush;
    for(HtmlParser reference: htmlRefrences) {
        if(reference.searchInElement("class=\"external text\" href=\"", linksToPush)) { 
            elementsToPush.push_back(linksToPush);
            allElements.push_back(elementsToPush);
            elementsToPush.clear();
        }
    }
}

void SourceRetriever::parseLinksAndCitations() {
    htmlRefrences.clear();
    allElements.clear();

    if(htmlRefrences.empty()) parseReferences();
    ////////* HTML Parser Code *////////
    // Searches for all the links within each refrence
    std::vector<std::vector<std::string>> elementsToPush;
    std::vector<std::string> linksToPush;
    std::vector<std::string> citationsToPush;
    for(HtmlParser reference: htmlRefrences) {
        if(reference.searchInElement("class=\"external text\" href=\"", linksToPush) && reference.searchInElement(citationsToPush)) { 
            // Push to individual reference element to push
            if(!citationsToPush.empty()) elementsToPush.push_back(citationsToPush);
            if(!linksToPush.empty()) elementsToPush.push_back(linksToPush); 
            allElements.push_back(elementsToPush); 
            elementsToPush.clear();
        }
    }
}

bool SourceRetriever::parseReferences() {
    bool output = false;
    ////////* HTML Parser Code *////////
    HtmlParser htmlParser("scraped0.html");
    htmlParser.readHTMLFile();

    // Refines looking for the references class
    htmlParser.refineSearch("class=\"references\"", HtmlParser::REF_ALL);

    // Extracts each element with <li in it
    HtmlParser htmlToPush;
    while(htmlParser.extractElement("<li", htmlToPush)) {
        output = true;
        htmlRefrences.push_back(htmlToPush);
    }
    return output;
}

void SourceRetriever::print(const SourceData& componentVector, const std::string& componentName) {
    ////////* Random Functions *////////
    for(int i = 0; i < componentVector.size(); i++) {
        const std::vector<std::vector<std::string>>& element = componentVector[i];
        std::cout << componentName << i + 1 << std::endl;
        for(int l = 0; l < element.size(); l++) {
            const std::vector<std::string>& subelement = element[l];
            std::cout << "  sub-Element " << l + 1 << ":" << std::endl;
            for(std::string item: subelement) {
                std::cout << "    - "<< item << std::endl;
            }
        }
        std::cout << "----------------\n";
    }
}

void SourceRetriever::printElements(void) {
    print(allElements, "Reference #");
}