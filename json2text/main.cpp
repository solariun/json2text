 //  main.cpp
//  json2text
//
//  Created by GUSTAVO CAMPOS on 06/03/18.
//  Copyright © 2018 GUSTAVO CAMPOS. All rights reserved.
//
/*
 MIT License
 
 Copyright (c) [year] [fullname]
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <errno.h>

#include "JsonParser.hpp"

using namespace std;



int main(int argc, const char * argv[]) {
    
    nice (19);
    
    char chCinbuffer [64 * 1024]; // 64k buffer;
    
    /*
     try
     {
     cout << "init text verify " << endl;
     
     jsonParserEx::verify(false, 10, "TESTE ");
     
     }
     catch (MetaException* ex)
     {
     cout << "Exception type (" << ex->what() << ") : " << ex->getExMessage() << endl;
     }
     */
    
    istream* isInput;
    
    ifstream* ifsFile;
    
    if (isatty (fileno(stdin)))
    {
        cout << "This applicacion works with pipe only." << endl;
        
        string strDataBuffer = "";
        
        if (argc != 2)
        {
            cout << "json to Text - " << to_string(argc) << endl;
            cout << "By Gustavo Campos (2018)" << endl << endl;
            cout << "Use by piping data or ./json2txt <file>" << endl << endl;
            
            exit (0);
        }
        
        ifsFile = new ifstream (argv[1]);
        //int errn = errno;
        
        if ((ifsFile->rdstate() & std::ifstream::failbit) != 0)
        {
            cerr << "Error: " << strerror(errno);
        }
        
        isInput = ifsFile;
        
    }
    else
    {
        cin.rdbuf()->pubsetbuf(chCinbuffer, sizeof (chCinbuffer));
        
        isInput = &cin;
    }
    
    jsonParser lexParser (*isInput);
    
    
    try
    {
        /*
        while ((lexParser.getNextxpathLikeItem(iteractor)) != NULL)
        {
            cout << iteractor.strDataPath << " = " << iteractor.strDataValue << endl;
        }
        */
        
        lexParser.dumpjsonAsText (cout);
        
    }
    catch (MetaException* ex)
    {
        cerr << "Exception " << ex->what() << " - " << ex->getExMessage() << endl;
    }
    catch (exception& ex)
    {
        cerr << "Exception " << ex.what();
    }

    cout << "Final processing" << endl;
    
    
    return 0;
}

