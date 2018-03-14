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
{cerr << "init class: " << typeid(this).name() << endl; }


jsonParserITemRet* jsonParser::getNextLexicalItem (jsonParserITemRet& jsonLExRet)
{
    string  strData = "";
    
    char    chChar = '\0';
    bool    bAddNext = false;
    
    if (isIn.eof()) return NULL;
    
    while (isIn.good())
    {
        chChar = isIn.get();
        
        //cerr << chChar << " tp: " << nType << " : String: [" << strData << "]" <<  " EOF: " << isIn.eof () << endl;
        
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
            return jsonLExRet.assign (limit_tag, strData);
        }
        else if (nType == none_tag && chChar == '"')
        {
            nType = string_quote_tag;
        }
        else if (nType == string_quote_tag && chChar == '"' && bAddNext == false)
        {
            //cerr << "Return \"\" string" << endl;
            
            nType = none_tag;
            return jsonLExRet.assign(string_tag, strData);
        }
        else if (nType == string_quote_tag && chChar == '\\' && bAddNext == false)
        {
            bAddNext = true;
        }
        else if (nType == none_tag && !isBetween (chChar, LX_DIVISION, sizeof (LX_DIVISION)-1) )
        {
            //cerr << "setting simple string...." << endl;
            
            isIn.putback(chChar);
            
            nType = string_tag;
        }
        else if (nType == string_tag && isBetween(chChar, "{}[]:,\" \r\n\t ", 11))
        {
            if (isBetween(chChar, "{}[]:,\"\"", 8))
            {
                isIn.putback(chChar);
            }
            
            //cerr << "Return simple srtring " << endl;
            
            nType = none_tag;
            return jsonLExRet.assign(string_tag, strData);
        }
        else if (nType == string_quote_tag || nType == string_tag || nType == value_tag)
        {
            if (bAddNext == true) bAddNext = false;
            
            //cerr << "adding [" << chChar << "] - type: " << nType << endl;
            
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
    
    cerr << endl << "New Path: [" << context.strPath << "] VariableName.size: [" << context.strVariableName.size() << "]" << endl<<endl;

    context.strVariableName = "";
}

void jsonParser::popPath  (jsonToTextContext& context)
{
    size_t nLastSlash = context.strPath.find_last_of("/");
    
    if (nLastSlash == string::npos)
        return;
    
    cerr << endl;
    cerr << "Actual Path: [" << context.strPath << "(" << context.strPath.size() << ", " << nLastSlash << "," << context.strPath [nLastSlash] << ")" << endl;
    
    context.strPath.resize(nLastSlash);
    if (context.nCurrentLevel >= context.nMinimalLevel) context.nCurrentLevel--;
    
    cerr << "Old Path: [" << context.strPath << "]" << endl << endl;
}


void jsonParser::addArrayToDataPath (jsonToTextContext& context)
{
    if (context.nMinimalLevel > 0)
    {
        stringstream strsValue;

        strsValue << "[" << context.nArrayCounter << "." << context.nArrayItemCounter << "]";
        
        context.strDataPath = context.strDataPath + strsValue.str();
        
        //cerr << "Added Array data: " << context.strDataPath << endl;
    }

    //context.strDataPath += "." + std::to_string(context.nArrayItemCounter) + "." + std::to_string(context.nArrayCounter);
}





jsonToTextContext* jsonParser::getNextxpathLikeItem (jsonToTextContext& context)
{
    //jsonParserEx::verify(strReturn != NULL, 10, "Error, the String variable is invalid. (NULL)");
    
    jsonParserITemRet jsonLexRet;
        
    while (getNextLexicalItem(jsonLexRet) != NULL)
    {
        cerr << "received : " << jsonLexRet.jsoneType << " : " << jsonLexRet.strValue << "; -->      nCurrentLevel: " << context.nCurrentLevel <<"; nMinimalLevel: " << context.nMinimalLevel << "; Array Counter: " << context.nArrayCounter << endl;
    
        
        if (jsonLexRet.jsoneType == open_struct_tag)
        {
            if (context.nStatus != init_tag && context.strVariableName.size()>0) pushPath (context);
            
            context.nStatus = none_tag;
        }
        else if (context.nCurrentLevel >= context.nMinimalLevel && jsonLexRet.jsoneType == close_struct_tag )
        {
            cerr << "Closing struct: nCurrentLevel: " << context.nCurrentLevel <<"; nMinimalLevel: " << context.nMinimalLevel << "; Array Counter: " << context.nArrayCounter << endl;
            
            if (context.nCurrentLevel > context.nMinimalLevel) popPath (context);
            
            if (context.nCurrentLevel > 0) context.nArrayItemCounter++;
            
            context.strVariableName = "";
        }
        else if ((context.nStatus == value_tag || context.nStatus == none_tag) && jsonLexRet.jsoneType == open_array_tag)
        {
            context.nStatus = none_tag;
            
            //cerr << "Open Array: nCurrentLevel: " << context.nCurrentLevel <<"; nMinimalLevel: " << context.nMinimalLevel << "; Array Counter: " << context.nArrayCounter << endl;

            context.lifoArrayLimits.push (context.nCurrentLevel);
            context.lifoArrayCounters.push (context.nArrayItemCounter);
            
            context.nArrayItemCounter = 1;
            
            pushPath (context);
            
            context.nMinimalLevel = context.nCurrentLevel;
            
            context.bArrayOn = true;
            
            context.nArrayCounter++;
        }
        else if (context.nStatus == attributive_tag && (jsonLexRet.jsoneType == set_tag))
        {
            context.nStatus = value_tag;
        }
        else if ((context.nStatus == none_tag || context.nStatus == attributive_tag) && jsonLexRet.jsoneType == close_array_tag)
        {
            bool brMustReturn = false;
            
            VERIFY(context.lifoArrayLimits.empty() == false, 10, "Error, Array controler lifo empty.");
            
            cerr << "Open Array: Peering:  nMinimalLevel: " << context.lifoArrayLimits.top() << "; nArrayItemCounter: " << context.lifoArrayCounters.top() << endl;
            
            context.nMinimalLevel = context.lifoArrayLimits.top();
            context.lifoArrayLimits.pop(); //cleaning the last item pushed
            
            cerr << "Open Array: nCurrentLevel: " << context.nCurrentLevel <<"; nMinimalLevel: " << context.nMinimalLevel << "; Array Counter: " << context.nArrayCounter << endl;
            
            if (context.nStatus == attributive_tag)
            {
                context.strVariableName.size();
                
                context.strDataValue =  context.strVariableName;
                context.strDataPath  =   context.strPath;
                context.strVariableName = "";
                
                addArrayToDataPath (context);
                
                brMustReturn = true;
            }
            
            context.nArrayItemCounter = context.lifoArrayCounters.top();
            context.lifoArrayCounters.pop();
            
            popPath(context);
            
            
            context.nStatus = none_tag;
            
            context.bArrayOn = false;
            
            context.strVariableName = "";
            
            if (brMustReturn == true)
            {
                return &context;
            }
        }
        else if (context.nStatus == none_tag && jsonLexRet.jsoneType == string_tag)
        {
            context.strVariableName = jsonLexRet.strValue;
            
            //VERIFY (getNextLexicalItem(jsonLexRet) != NULL && jsonLexRet.jsoneType == set_tag, toText_Set_Expected, "Error, no set char fond.");
            
            context.nStatus = attributive_tag;
        }
        else if (context.nStatus == attributive_tag)
        {
            context.strDataValue =  context.strVariableName;
            context.strDataPath  =   context.strPath;
            
            addArrayToDataPath (context);
            
            if (context.nArrayItemCounter> 0) context.nArrayItemCounter++;
            
            context.strVariableName = "";
            
            context.nStatus = none_tag;
            context.strVariableName = "";
            
            return &context;
        }
        else if (context.nStatus == value_tag && jsonLexRet.jsoneType == string_tag)
        {
            context.strDataValue =  jsonLexRet.strValue;
            context.strDataPath  =   context.strPath + "/" + context.strVariableName;
            addArrayToDataPath (context);

            context.nStatus = none_tag;
            
            context.strVariableName="";
            return &context;
        }
    };
    
    return NULL;
}


void jsonParser::dumpjsonAsText (ostream& osOutput)
{
    string strPath;
    
    dumpjsonAsText (osOutput, init_tag, &strPath);
}


void jsonParser::pushPath (string& strPath, string& strNew)
{
    strPath +=  "/" + strNew;

    strNew = "";
}

void jsonParser::popPath  (string& strPath)
{
    size_t nLastSlash = strPath.find_last_of("/");
    
    if (nLastSlash == string::npos)
        return;
    
    cerr << endl;
    cerr << "Actual Path: [" << strPath << "(" << strPath.size() << ", " << nLastSlash << "," << strPath [nLastSlash] << ")" << endl;
    
    strPath.resize(nLastSlash);
    
    cerr << "Previous accessed: [" << strPath << "]" << endl << endl;
}

#define prt_tag(x,y) cerr << x << "=" << y << endl

void jsonParser::dumpjsonAsText (ostream& osOutput, jsonElements_t nStatus, string* strPath)
{
    jsonParserITemRet jsonLexRet;
    jsonElements_t nWorking = none_tag;
    
    uint32_t  nLevels = 0;
    
    string strValue = "";
    
    
    if (nStatus == named_struct_tag || nStatus == struct_tag || nStatus == array_tag)
    {
        nWorking = nStatus;
        
        nStatus = none_tag;
    }
    
    while (getNextLexicalItem(jsonLexRet) != NULL)
    {
        if (nStatus == none_tag && strValue.size() > 0)
        {
            strValue = "";
        }
        
        if (jsonLexRet.jsoneType == open_struct_tag)
        {
            if (nStatus != init_tag)
            {
                jsonElements_t tempStatus = struct_tag;
                
                if (nStatus == attributive_tag && strValue.size()>0)
                {
                    pushPath(*strPath, strValue);
                    nLevels++;
                    
                    tempStatus = named_struct_tag;
                }

                dumpjsonAsText (osOutput, tempStatus, strPath);
            }
            
            nStatus = none_tag;
            
            continue;
        }
        else if (jsonLexRet.jsoneType == open_array_tag)
        {
            VERIFY ( nStatus == attributive_tag && strValue.size() > 0, 11,
                    "Errro, the Array MUST be after an attributive session.");
            
            pushPath(*strPath, strValue);
            nLevels++;
            
            dumpjsonAsText (osOutput, array_tag, strPath);
            
            continue;
        }
        else if (nStatus == none_tag && jsonLexRet.jsoneType == string_tag)
        {
            strValue = jsonLexRet.strValue;
            nStatus = string_tag;
            
            continue;
        }
        else if (nStatus == string_tag)
        {
            if (jsonLexRet.jsoneType == set_tag)
            {
                nStatus = attributive_tag;
                
                continue;
            }
            else if (jsonLexRet.jsoneType == limit_tag)
            {
                nStatus = value_tag;
            }
        }
        
        
        if (nWorking == struct_tag || nWorking == named_struct_tag)
        {
            
            if (nStatus == value_tag)
            {
                prt_tag(*strPath << "none", strValue);
            }
            else if (nStatus == attributive_tag)
            {
                VERIFY (jsonLexRet.jsoneType == string_tag, 13, "Error, no associated DATA.");
                
                prt_tag(*strPath << "/" << strValue, jsonLexRet.strValue);
            }
            else if (jsonLexRet.jsoneType == close_struct_tag)
            {
                VERIFY (nStatus == none_tag, 12, "Error, closing struct must be CLEAR.");
                
                if (nWorking == named_struct_tag)
                {
                    popPath(*strPath);
                }
                
                return;
            }
            
            nStatus = none_tag;
            
        }
        else if (nWorking == array_tag)
        {
            
            if (nStatus == value_tag)
            {
                prt_tag(*strPath, jsonLexRet.strValue);
            }
            else if (nStatus == attributive_tag)
            {
                VERIFY (jsonLexRet.jsoneType == string_tag, 13, "Error, no associated DATA.");
                
                prt_tag(*strPath << "/" << strValue, jsonLexRet.strValue);
            }
            else if (jsonLexRet.jsoneType == close_array_tag)
            {
                if (nStatus == string_tag)
                {
                    prt_tag(*strPath, jsonLexRet.strValue);
                }
                
                popPath(*strPath);
                
                return;
            }
            nStatus = none_tag;
        }
    }
}


//0103182186491
