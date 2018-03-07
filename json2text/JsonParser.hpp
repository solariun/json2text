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
#include <queue>
#include "MetaException.hpp"

using namespace std;

/*
 Excpetion habler
 */
class jsonParserEx : public MetaException
{
private:
    uint32_t n32Code;
    string   strMessage;
    
    jsonParserEx();
    
public:
    
    jsonParserEx (uint32_t n32Code, string strMessage);
    
    virtual const char* what() const throw()
    {
        return "jsonParser::jsonParserEx";
    }
};


/*
 * ERROR CODE
 */

enum jsonErrorCodes
{
    toText_Variable_Expceted,
    toText_Set_Expected
};

/*
 * Enum used to typing the Lexical Itens.
 */

enum jsonElements_t
{
    none_tag,
    open_struct_tag,
    close_struct_tag,
    attribure_tag,
    set_tag,
    value_tag,
    string_quote_tag,
    string_tag,
    comma_tag,
    open_array_tag,
    close_array_tag
};



/*
 * Type used for returning the Lexical ITem
 */

class jsonDataITem
{
public:
    jsonDataITem(jsonElements_t  jsoneType, string& strAttribute, string& strValue) : jsoneType(jsoneType), strValue(strValue), strAttribute(strAttribute){};
    
    jsonDataITem(){};
    
    jsonDataITem* assign (jsonElements_t  jsoneType, string& strAttribute, string& strValue)
    {
        this->jsoneType = jsoneType;
        this->strValue = strValue;
        this->strAttribute = strAttribute;
        
        return this;
    }
    
    jsonElements_t   jsoneType = none_tag;
    string           strValue = "";
    string           strAttribute = "";
};

/*
 * Type used for json for text interactions
 */

class jsonToTextitereactor
{
public:
    string      strPath = "";
    string      strVariableName = "";
    size_t      nLevels=0;
    size_t      nMinimalLevel=0;
    bool        bArrayOn;
    
    queue<size_t> nArrayQueue;
    
    jsonElements_t nStatus = none_tag;
    
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

    
private:
    jsonParser ();
    
    jsonParserITemRet* getNewjsonParserITemRet(jsonElements_t  jsoneType, string strValue);

    jsonParserITemRet* getNextLexicalItem (jsonParserITemRet& strData);

public:
    
    jsonParser (istream& isIn);
    
    jsonToTextitereactor* getNextxpathLikeItem (jsonToTextitereactor& itereactor);
    
};


#endif /* JsonParser_hpp */