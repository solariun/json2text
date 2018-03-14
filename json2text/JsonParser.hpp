//
//  JsonParser.hpp
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


#ifndef JsonParser_hpp
#define JsonParser_hpp


#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include "MetaException.hpp"

using namespace std;

/*
 * ERROR CODE
 */

enum jsonErrorCodes
{
    toText_Variable_Expceted,
    toText_Set_Expected,
    toText_Array_lifo_Enpty
};

/*
 * Enum used to typing the Lexical Itens.
 */

enum jsonElements_t
{
    init_tag,
    none_tag,
    struct_tag,
    open_struct_tag,
    close_struct_tag,
    attributive_tag,
    set_tag,
    value_tag,
    string_quote_tag,
    string_tag,
    limit_tag,
    array_tag,
    open_array_tag,
    close_array_tag
};



/*
 * Type used for returning the Lexical ITem
 */

class jsonParserITemRet
{
public:
    jsonParserITemRet(jsonElements_t  jsoneType, string& strValue) : jsoneType(jsoneType), strValue(strValue){cout << "init class: " << typeid(this).name() << endl; };
    
    jsonParserITemRet(){};
    
    jsonParserITemRet* assign (jsonElements_t  jsoneType, string& strValue)
    {
        this->jsoneType = jsoneType;
        this->strValue = strValue;
        
        return this;
    }
    
    jsonElements_t   jsoneType = none_tag;
    string           strValue = "";
    string           strAttribute = "";
};

/*
 * Type used for json for text interactions
 */

class jsonToTextContext
{
public:
    string      strPath = "";
    string      strVariableName = "";
    size_t      nCurrentLevel=0;
    size_t      nMinimalLevel=0;
    size_t      nArrayItemCounter = 0;
    size_t      nArrayCounter=0;
    
    bool        bArrayOn;
    
    stack<size_t>  lifoArrayLimits;
    stack<size_t>  lifoArrayCounters;
    
    jsonElements_t nStatus = init_tag;
    
    string strDataPath = "";
    string strDataValue= "";
    
};

/*
 * Implementation Prototypes
 */

class jsonParser
{
protected:
    istream& isIn = cin;
    jsonElements_t nType = none_tag;
    void addArrayToDataPath (jsonToTextContext& context);
    void dumpjsonAsText (ostream& osOutput, jsonElements_t nStatus, string* strPath);
    
private:
    jsonParser ();
    
    jsonParserITemRet* getNewjsonParserITemRet(jsonElements_t  jsoneType, string strValue);

    jsonParserITemRet* getNextLexicalItem (jsonParserITemRet& strData);

    void pushPath (jsonToTextContext& context);
    void popPath  (jsonToTextContext& context);

    void pushPath (string& strPath, string& strNew);
    void popPath  (string& strPath);

public:
    
    jsonParser (istream& isIn);
    
    jsonToTextContext* getNextxpathLikeItem (jsonToTextContext& itereactor);
    
    void dumpjsonAsText (ostream& osOutput);
};


#endif /* JsonParser_hpp */
