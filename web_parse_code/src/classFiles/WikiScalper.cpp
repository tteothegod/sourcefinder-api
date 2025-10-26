#include "WikiScalper.h"

int WikiScalper::htmlCount = 0;

// TODO: work on recieving input and parsing string to append working directory and then sperate file name from it.
void WikiScalper::initialize()
{
    Py_Initialize(); 
    
    //Set directory to current working directory
    PyRun_SimpleString("import sys; import os; sys.path.append(os.getcwd() + '/src/classFiles')");

    // Import Python module
    PyObject* pModule = PyImport_ImportModule(pyFileName.c_str());
    if (pModule == nullptr) 
    {
        PyErr_Print();
        Py_Finalize();
        throw std::invalid_argument("Program was unable to import py scalper script");
    }

    // Get function reference
    pFunc = PyObject_GetAttrString(pModule, "fetch_and_save_html");
    if (pFunc == nullptr || !PyCallable_Check(pFunc)) 
    {
        PyErr_Print();
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
        Py_Finalize();
        throw std::invalid_argument("Cannot find function 'scrape_website'.");
    }
}

void WikiScalper::runScript()
{
    // Generate a dynamic filename based on htmlCount
    std::string outputFile = "scraped" + std::to_string(htmlCount) + ".html";

    // Call Python function with URL and output filename
    PyObject* pArgs = PyTuple_Pack(2, PyUnicode_FromString(inputURL.c_str()), PyUnicode_FromString(outputFile.c_str()));
    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);

    if (pResult == nullptr) 
    {
        PyErr_Print();
        throw std::invalid_argument("Python function execution failed.");
    }

    // Check if Python returned None
    if (pResult == Py_None) 
    {
        std::cout << "Python script did not find a valid link. Stopping execution.\n";
        Py_DECREF(pResult);
        exit(1);  // Exit the program
    }

    // Increment counter for next file
    htmlCount++;
    Py_DECREF(pResult);
}

