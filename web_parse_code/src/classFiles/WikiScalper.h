#ifndef WIKI_SCALPER_H
#define WIKI_SCALPER_H
#include <iostream>
#include <Python.h>



class WikiScalper
{
    public:
    static int htmlCount;
    WikiScalper(void) {}
    WikiScalper(const std::string& fileToRun): pyFileName(fileToRun) { initialize(); }
    void initialize();
    void runScript();
    void setScript(const std::string& script) { pyFileName = script; }
    void setURL(const std::string& url) { inputURL = url; }

    private:
    PyObject *pFunc;
    std::string pyFileName;
    std::string inputURL;
};

#endif