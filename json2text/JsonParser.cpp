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
#include <sstream>


#define LX_OTIONS "!-/=! "
#define LX_DIVISION " \r\n\t"

/*
 Lexical Parser Functions
 */

jsonParser::jsonParser ()
{}


jsonParser::jsonParser (istream& isIn) : isIn (isIn)
{cout << "init class: " << typeid(this).name() << endl; }


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
            
            /*
             if (chChar == '\n')
                strData += "\n";
            else if (chChar == '\r')
                strData += "\r";
            else
             */
             
            if (chChar >= ' ') strData += chChar;
            
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


void jsonParser::pushPath (jsonToTextContext& context)
{
    context.strPath +=  "/" + context.strVariableName;
    
    context.nCurrentLevel++;
    
    //cout << endl << "New Path: [" << context.strPath << "] VariableName.size: [" << context.strVariableName.size() << "]" << endl<<endl;

}

void jsonParser::popPath  (jsonToTextContext& context)
{
    size_t nLastSlash = context.strPath.find_last_of("/");
    
    if (nLastSlash == string::npos)
        return;
    
    //cout << endl;
    //cout << "Actual Path: [" << context.strPath << "(" << context.strPath.size() << ", " << nLastSlash << "," << context.strPath [nLastSlash] << ")" << endl;
    
    context.strPath.resize(nLastSlash);
    context.nCurrentLevel--;
    
    //cout << "Old Path: [" << context.strPath << "]" << endl << endl;
}


void jsonParser::addArrayToDataPath (jsonToTextContext& context)
{
    if (context.nMinimalLevel > 0)
    {
        stringstream strsValue;

        strsValue << "[" << context.nArrayCounter << "." << context.nArrayItemCounter << "]";
        
        context.strDataPath = context.strDataPath + strsValue.str();
        
        //cout << "Added Array data: " << context.strDataPath << endl;
    }

    //context.strDataPath += "." + std::to_string(context.nArrayItemCounter) + "." + std::to_string(context.nArrayCounter);
}


jsonToTextContext* jsonParser::getNextxpathLikeItem (jsonToTextContext& context)
{
    //jsonParserEx::verify(strReturn != NULL, 10, "Error, the String variable is invalid. (NULL)");
    
    jsonParserITemRet jsonLexRet;
    
    
    while (getNextLexicalItem(jsonLexRet) != NULL)
    {
        //cout << "received : " << jsonLexRet.jsoneType << " : " << jsonLexRet.strValue << endl;
    
        if (context.nArrayCounter == 3790 && context.nArrayItemCounter == 1)
        {
            cout << "";
        }
        
        if (/*context.nStatus != none_tag && */jsonLexRet.jsoneType == open_struct_tag)
        {
            if (context.nStatus == value_tag)
            {
                pushPath (context);
                
                context.nStatus = none_tag;
                
            }
            
            if (context.nCurrentLevel > 0) context.nArrayItemCounter++;
        }
        else if (context.nCurrentLevel >= context.nMinimalLevel && jsonLexRet.jsoneType == close_struct_tag )
        {
            popPath (context);
        }
        else if ((context.nStatus == value_tag || context.nStatus == none_tag) && jsonLexRet.jsoneType == open_array_tag)
        {
            context.nStatus = none_tag;
            
            context.queueArrayLimits.push (context.nCurrentLevel);
            context.queueArrayCounters.push (context.nArrayItemCounter);
            
            context.nArrayItemCounter = 1;
            
            pushPath (context);
            
            context.nMinimalLevel = context.nCurrentLevel;
            
            context.bArrayOn = true;
            
            context.nArrayCounter++;
        }
        else if (context.nStatus == attribute_tag && (jsonLexRet.jsoneType == set_tag))
        {
            context.nStatus = value_tag;
        }
        else if ((context.nStatus == none_tag || context.nStatus == attribute_tag) && jsonLexRet.jsoneType == close_array_tag)
        {
            bool brMustReturn = false;
            
            VERIFY(context.queueArrayLimits.empty() == false, 10, "Error, Array controler queue empty.");
            
            context.nMinimalLevel = context.queueArrayLimits.front();
            context.queueArrayLimits.pop(); //cleaning the last item pushed
            
            if (context.nStatus == attribute_tag)
            {
                context.strVariableName.size();
                
                context.strDataValue =  context.strVariableName;
                context.strDataPath  =   context.strPath;
                context.strVariableName = "";
                
                addArrayToDataPath (context);
                
                brMustReturn = true;
            }
            
            context.nArrayItemCounter = context.queueArrayCounters.front();
            context.queueArrayCounters.pop();
            
            popPath(context);
            
            context.nStatus = none_tag;
            
            context.bArrayOn = false;
            
            if (brMustReturn == true)
            {
                return &context;
            }
        }
        else if (context.nStatus == none_tag && jsonLexRet.jsoneType == string_tag)
        {
            context.strVariableName = jsonLexRet.strValue;
            
            //VERIFY (getNextLexicalItem(jsonLexRet) != NULL && jsonLexRet.jsoneType == set_tag, toText_Set_Expected, "Error, no set char fond.");
            
            context.nStatus = attribute_tag;
        }
        else if (context.nStatus == attribute_tag)
        {
            context.strDataValue =  context.strVariableName;
            context.strDataPath  =   context.strPath;
            
            addArrayToDataPath (context);
            if (context.nArrayItemCounter> 0) context.nArrayItemCounter++;
            
            context.strVariableName = "";
            
            context.nStatus = none_tag;
            return &context;
        }
        else if (context.nStatus == value_tag && jsonLexRet.jsoneType == string_tag)
        {
            context.strDataValue =  jsonLexRet.strValue;
            context.strDataPath  =   context.strPath + "/" + context.strVariableName;
            addArrayToDataPath (context);

            context.nStatus = none_tag;
            return &context;
        }
    };
    
    return NULL;
}


//0103182186491
