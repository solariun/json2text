//
//  JsonParser.cpp
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


#include "JsonParser.hpp"


#include "Util.hpp"

#include <iostream>
#include <new>


#define LX_OTIONS "!-/=!"
#define LX_DIVISION " \r\n\t"
/*
 Exception Handler
 */

jsonParserEx::jsonParserEx () : MetaException(0, "")
{}


jsonParserEx::jsonParserEx (uint32_t n32Code, string strMessage) : MetaException(n32Code, strMessage)
{}


/*
 Lexical Parser Functions
 */

jsonParser::jsonParser ()
{}


jsonParser::jsonParser (istream& isIn) : isIn (isIn)
{}


jsonParserITemRet* jsonParser::getNextLexicalItem (jsonParserITemRet& jsonLExRet)
{
    string  strData = "";
    
    char    chChar = '\0';
    bool    bAddNext = false;
    
    if (isIn.eof()) return NULL;
    
    while (isIn.good())
    {
        chChar = isIn.get();
        
        //cout << chChar << " tp: " << nType << " : String: [" << strData << "]" <<  " EOF: " << isIn.eof () << endl;
        
        if (nType == none_tag && chChar == '{')
        {
            strData = chChar;
            
            nType = none_tag;
            return jsonLExRet.assign (open_struct_tag, strData);
        }
        else if (nType == none_tag && chChar == '}')
        {
            strData = chChar;
            
            nType = none_tag;
            return jsonLExRet.assign (close_struct_tag, strData);
        }
        else if (nType == none_tag && chChar == '[')
        {
            strData = chChar;
            
            nType = none_tag;
            return jsonLExRet.assign (open_array_tag, strData);
        }
        else if (nType == none_tag && chChar == ']')
        {
            strData = chChar;
            
            nType = none_tag;
            return jsonLExRet.assign (close_array_tag, strData);
        }
        else if (nType == none_tag && chChar == ':')
        {
            strData = chChar;
            
            nType = none_tag;
            return jsonLExRet.assign (set_tag, strData);
        }
        else if (nType == none_tag && chChar == ',')
        {
            strData = chChar;
            
            nType = none_tag;
            return jsonLExRet.assign (comma_tag, strData);
        }
        else if (nType == none_tag && chChar == '"')
        {
            nType = string_quote_tag;
        }
        else if (nType == string_quote_tag && chChar == '"' && bAddNext == false)
        {
            //cout << "Return \"\" string" << endl;
            
            nType = none_tag;
            return jsonLExRet.assign(string_tag, strData);
        }
        else if (nType == string_quote_tag && chChar == '\\' && bAddNext == false)
        {
            bAddNext = true;
        }
        else if (nType == none_tag && !isBetween (chChar, LX_DIVISION, sizeof (LX_DIVISION)-1) )
        {
            //cout << "setting simple string...." << endl;
            
            isIn.putback(chChar);
            
            nType = string_tag;
        }
        else if (nType == string_tag && isBetween(chChar, "{}[]:,\" \r\n\t ", 11))
        {
            if (isBetween(chChar, "{}[]:,\"\"", 8))
            {
                isIn.putback(chChar);
            }
            
            //cout << "Return simple srtring " << endl;
            
            nType = none_tag;
            return jsonLExRet.assign(string_tag, strData);
        }
        else if (nType == string_quote_tag || nType == string_tag || nType == value_tag)
        {
            if (bAddNext == true) bAddNext = false;
            
            //cout << "adding [" << chChar << "] - type: " << nType << endl;
            
            if (chChar == '\n')
                strData += "\n";
            else if (chChar == '\r')
                strData += "\r";
            else
                strData += chChar;
            
        }
    }
    
    return NULL;
}

jsonParserITemRet* jsonParser::getNewjsonParserITemRet(jsonElements_t  jsoneType, string strValue)
{
    try
    {
        return new jsonParserITemRet (jsoneType, strValue);
        
    } catch (std::bad_alloc& ex)
    {
        cerr << __PRETTY_FUNCTION__ << " exception: " << ex.what();
        
        throw ex;
    }
    
    return NULL;
}


jsonToTextitereactor* jsonParser::getNextxpathLikeItem (jsonToTextitereactor& itereactor)
{
    //jsonParserEx::verify(strReturn != NULL, 10, "Error, the String variable is invalid. (NULL)");
    
    jsonParserITemRet jsonLexRet;

    
    
    while (getNextLexicalItem(jsonLexRet) != NULL)
    {
        //cout << "received : " << jsonLexRet.jsoneType << " : " << jsonLexRet.strValue << endl;
        
        if (itereactor.nStatus != none_tag && jsonLexRet.jsoneType == open_struct_tag)
        {
            if (itereactor.nStatus == value_tag)
            {
                itereactor.strPath +=  "/" + itereactor.strVariableName;
                
                //cout << "New Path: [" << itereactor.strPath << "]" << endl;
                
                itereactor.nLevels++;
                
                itereactor.nStatus = none_tag;
            }
        }
        else if (jsonLexRet.jsoneType == close_struct_tag)
        {
            size_t nLastSlash = itereactor.strPath.find_last_of("/");
            
            //cout << endl;
            //cout << "Actual Path: [" << itereactor.strPath << "(" << itereactor.strPath.size() << ", " << nLastSlash << "," << itereactor.strPath [nLastSlash] << ")" << endl;
            
            itereactor.strPath.resize(nLastSlash);
            
            //cout << "Old Path: [" << itereactor.strPath << "]" << endl;
            
            itereactor.nLevels--;
        }
        else if (itereactor.nStatus == value_tag && jsonLexRet.jsoneType == open_array_tag)
        {
            itereactor.nStatus = none_tag;
            
        }
        else if (itereactor.nStatus == value_tag && jsonLexRet.jsoneType == close_array_tag)
        {
            itereactor.nStatus = none_tag;
        }
        else if (itereactor.nStatus == none_tag && jsonLexRet.jsoneType == string_tag)
        {
            itereactor.strVariableName = jsonLexRet.strValue;
            
            jsonParserEx::verify(getNextLexicalItem(jsonLexRet) != NULL && jsonLexRet.jsoneType == set_tag, toText_Set_Expected, "Error, no set char fond.");
            
            itereactor.nStatus = value_tag;
        }
        else if (itereactor.nStatus == value_tag && jsonLexRet.jsoneType == string_tag)
        {
            itereactor.strDataValue =  jsonLexRet.strValue;
            itereactor.strDataPath  =   itereactor.strPath + "/" + itereactor.strVariableName;
            
            itereactor.nStatus = none_tag;
            return &itereactor;
        }
    };
    
    return NULL;
}


//0103182186491
