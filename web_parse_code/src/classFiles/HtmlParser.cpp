#include "HtmlParser.h"

// TODO: Throw Eceptions:
// readHTMLFile() prints an error if the file is missing but continues execution.
// refineSearch() prints a message but doesn’t handle errors properly.
// extractElement() silently fails if no element is found.

////////* HTML Parser Code *////////
////////* Public *////////
void HtmlParser::readHTMLFile(const std::string &filename)
{
   htmlStrings.clear();
   std::ifstream file(filename); // Open the file
   if (!file)
   {
      std::cerr << "Error: Could not open file " << filename << std::endl;
   }
   std::string toPush;
   while (std::getline(file, toPush))
      htmlStrings.push_back(toPush);
}

bool HtmlParser::refineSearch(const std::string &strToFind)
{
   // Find iterators of element with strToFind
   int itStart, itEnd;
   findHTMLIterators(itStart, itEnd, strToFind);

   // If iterators are found then refine main HTML
   if (itStart != -1 && itEnd != -1 && itStart)
   {
      htmlStrings = std::vector<std::string>(htmlStrings.begin() + itStart, htmlStrings.begin() + itEnd + 1);
      return true;
   }
   else
   {
      std::cout << "IMORTANT: Your search was not found!\n";
      return false;
   }
}

bool HtmlParser::refineSearch(const std::string &strToFind, const RefineMethod &method)
{
   int itStart, itEnd;
   HtmlParser htmlCopy = HtmlParser(htmlStrings);
   std::vector<int> itStarts, itEnds;
   int accumIt = 0;

   std::vector<std::string> allHtmlSegments; // Store all matched HTML segments

   while (true)
   {
      htmlCopy.findHTMLIterators(itStart, itEnd, strToFind);
      if (itStart == -1 || itEnd == -1)
         break;

      itStarts.push_back(itStart + accumIt);
      itEnds.push_back(itEnd + accumIt);

      // Store the found segment
      std::vector<std::string> segment(htmlStrings.begin() + itStart + accumIt, htmlStrings.begin() + itEnd + 1 + accumIt);
      allHtmlSegments.insert(allHtmlSegments.end(), segment.begin(), segment.end());

      // Move past the found element to continue searching
      htmlCopy.htmlStrings = std::vector<std::string>(htmlCopy.htmlStrings.begin() + itEnd + 1, htmlCopy.htmlStrings.end());
      accumIt += itEnd + 1;
   }

   if (itStarts.empty())
      return false;

   switch (method)
   {
   case REF_LARGEST:
   {
      int maxDiff = 0, maxIndex = 0;
      for (int i = 0; i < itStarts.size(); i++)
      {
         int diff = itEnds[i] - itStarts[i];
         if (diff > maxDiff)
         {
            maxDiff = diff;
            maxIndex = i;
         }
      }
      itStart = itStarts[maxIndex];
      itEnd = itEnds[maxIndex];
      break;
   }

   case REF_SMALLEST:
   {
      int minDiff = INT_MAX, minIndex = 0;
      for (int i = 0; i < itStarts.size(); i++)
      {
         int diff = itEnds[i] - itStarts[i];
         if (diff < minDiff)
         {
            minDiff = diff;
            minIndex = i;
         }
      }
      itStart = itStarts[minIndex];
      itEnd = itEnds[minIndex];
      break;
   }

   case REF_ALL:
   {
      htmlStrings = allHtmlSegments; // Store all extracted HTML segments
      return true;
   }
   default:
      return false;
   }

   // If iterators are found, refine `htmlStrings`
   if (itStart != -1 && itEnd != -1)
   {
      htmlStrings = std::vector<std::string>(htmlStrings.begin() + itStart, htmlStrings.begin() + itEnd + 1);
      return true;
   }
   else
   {
      std::cout << "IMPORTANT: Your search was not found!\n";
      return false;
   }
}

bool HtmlParser::refineSearch(const std::string &strToFind, const std::string &tag)
{
   // Find iterators of element with strToFind
   int itStart, itEnd;
   findHTMLIterators(itStart, itEnd, strToFind, tag);

   // If iterators are found then refine main HTML
   if (itStart != -1 && itEnd != -1 && itStart)
   {
      std::vector<std::string> refined(htmlStrings.begin() + itStart, htmlStrings.begin() + itEnd + 1);
      htmlStrings = std::move(refined); // Transfers ownership without extra copying
      return true;
   }
   else
   {
      std::cout << "IMORTANT: Your search was not found!\n";
      return false;
   }
}

bool HtmlParser::extractElement(const std::string &strToFind, HtmlParser &extractTo)
{
   // Find iterators of element with strToFind
   int itStart, itEnd;
   findHTMLIterators(itStart, itEnd, strToFind);

   // If iterators are found then extract from main HTML
   if (itStart != -1 && itEnd != -1)
   {
      extractTo = HtmlParser(std::vector<std::string>(htmlStrings.begin() + itStart, htmlStrings.begin() + itEnd + 1));
      htmlStrings.erase(htmlStrings.begin() + itStart, htmlStrings.begin() + itEnd + 1);
      return true;
   }
   else
   {
      return false;
   }
}
bool HtmlParser::has(const std::string &str) const
{
   int s, e;
   findHTMLIterators(s, e, str);
   return (s != -1 && e != -1);
}
void HtmlParser::printHTML() const
{
   for (const std::string &line : htmlStrings)
   {
      std::cout << line << std::endl;
   }
}
void HtmlParser::manualPrint(const int &lines) const
{
   if (htmlStrings.size() >= lines)
   {
      for (int i = 0; i < lines; i++)
      {
         std::cout << htmlStrings[i] << std::endl;
      }
   }
   else
      std::cout << "HTML Not large enough\n";
}
bool HtmlParser::searchInElement(const std::string &strToFind, std::vector<std::string> &foundStrings) const
{
   // make a copy of parser
   std::vector<std::string> htmlStringsCopy = this->htmlStrings;
   foundStrings.clear();
   bool strFound = false;

   for (std::string line : htmlStringsCopy)
   {
      std::string strToPush;
      while (findSingleString(line, strToFind, strToPush))
      {
         foundStrings.push_back(strToPush);
         strFound = true;
      }
   }
   return strFound;
}
bool HtmlParser::searchInElement(std::vector<std::string> &foundStrings) const
{
   // make a copy of parser
   //  std::vector<std::string> htmlStringsCopy = this->htmlStrings;
   foundStrings.clear();
   bool strFound = false;

   std::string accumeStr = "";
   for (std::string line : htmlStrings)
   {
      if (line.find("<") == std::string::npos)
      {
         accumeStr += std::string(line.begin() + line.find_first_not_of(" "), line.end());
         strFound = true;
      }
   }
   foundStrings.push_back(accumeStr);

   return strFound;
}

std::vector<std::string> HtmlParser::searchInElement(const std::string &strToFind) const
{
   std::vector<std::string> foundStrings;
   searchInElement(strToFind, foundStrings);
   return foundStrings;
}

////////* Private *////////
void HtmlParser::findHTMLIterators(int &itStart, int &itEnd, const std::string &strToFind) const
{
   // Set Iterators to -1 to start (Iterators can never be -1)
   itStart = -1;
   itEnd = -1;
   std::string tagToEnd;
   for (int i = 0; i < htmlStrings.size(); i++)
   {
      std::string currLine = htmlStrings[i];
      // if start search hasnt been found
      if (itStart == -1)
      {
         size_t strPose = currLine.find(strToFind);
         if (strPose != std::string::npos)
         {
            itStart = i;
            // Extract the HTML tag name
            for (int l = strPose; l >= 0; l--)
            {
               if (currLine[l] == '<')
               {
                  int j = l + 1;
                  while (j < currLine.size() && currLine[j] != ' ' && currLine[j] != '>')
                  {
                     tagToEnd += currLine[j++];
                  }
                  break;
               }
            }
         }
      }
      // Find closing tag
      else if (itEnd == -1 && currLine.find("</" + tagToEnd) != std::string::npos)
      {
         itEnd = i;
      }
   }
}

void HtmlParser::findHTMLIterators(int &itStart, int &itEnd, const std::string &strToFind, const std::string &tagToEnd) const
{
   // Set Iterators to -1 to start (Iterators can never be -1)
   itStart = -1;
   itEnd = -1;
   for (int i = 0; i < htmlStrings.size(); i++)
   {
      std::string currLine = htmlStrings[i];
      // if start search hasnt been found
      if (itStart == -1)
      {
         size_t strPose = currLine.find(strToFind);
         if (strPose != std::string::npos)
         {
            itStart = i;
         }
      }
      // Find closing tag
      else if (itEnd == -1 && currLine.find("</" + tagToEnd) != std::string::npos)
      {
         itEnd = i;
      }
   }
}

bool HtmlParser::findSingleString(std::string &htmlLine, const std::string &strToFind, std::string &foundString) const
{
   size_t startIt, endIt;
   bool boolOutput = false;
   startIt = htmlLine.find(strToFind);
   if (startIt != std::string::npos)
   {
      endIt = std::string(htmlLine.begin() + startIt + strToFind.length(), htmlLine.end()).find("\"");
      if (endIt != std::string::npos)
      {
         foundString = std::string(htmlLine.begin() + startIt + strToFind.length(), htmlLine.begin() + startIt + strToFind.length() + endIt);
         htmlLine = std::string(htmlLine.begin() + startIt + strToFind.length() + endIt, htmlLine.end());
         boolOutput = true;
      }
   }
   return boolOutput;
}

void HtmlParser::deleteHtmlTags(std::string &htmlLine) const
{
   size_t startIt = htmlLine.find("<");
   size_t endIt = htmlLine.find(">");
   while (startIt != std::string::npos && endIt != std::string::npos)
   {
      htmlLine.erase(startIt, endIt - startIt + 1);
      startIt = htmlLine.find("<");
      endIt = htmlLine.find(">");
   }
}
