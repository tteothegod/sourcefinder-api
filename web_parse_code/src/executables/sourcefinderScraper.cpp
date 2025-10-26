#include <iostream>
#include <cstdlib>
#include "../classFiles/SourceRetriever.h"
#include <algorithm>




int main()
{

    ////////* Source Retriever *////////
    SourceRetriever retriever;
    retriever.setLinkMethod(SourceRetriever::LINK_INPUT);
    retriever.scalp();
    retriever.parseLinksAndCitations();
    SourceData elements = retriever.getElements();

    // Remove ^ from beginning of citations
    elements.removeSpecificChar('^', 0, 0, 0);

    // Credible website domains
    const std::vector<std::string> credibleDatabases = {
        "doi.org", "jstor.org", "springer.com", "sciencedirect.com",
        "researchgate.net", "avrxiv.org"
    };

    // Partition by credibility
    SourceData credibleSources = elements;
    SourceData remainingSources;
    credibleSources.partition( remainingSources, PartByCredibility(credibleDatabases, 1, false));

    SourceData possiblyCredibleSources = remainingSources;
    possiblyCredibleSources.partition( remainingSources, PartByConsistentChar('\"', 0, 0, true));

    // Print Statements
    // SourceRetriever::print( remainingSources, "Unfiltered Source #");
    SourceRetriever::print(possiblyCredibleSources, "Possibly Credible #");
    SourceRetriever::print(credibleSources, "Credible #");
    std::cout << "\nSummary of Sources:" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Credible Sources: " << creibleSources.size() << std::endl;
    std::cout << "Possibly Credible Sources: " << possiblyCredibleSources.size() << std::endl;
    std::cout << "Remaining Sources: " << remainingSources.size() << std::endl;
}