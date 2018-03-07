//
//  MetaException.cpp
//  xml2xpath
//
//  Created by GUSTAVO CAMPOS on 26/02/18.
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


#include "MetaException.hpp"
#include <sstream>


MetaException::MetaException(const uint32_t nExID,  const string& strValue) _NOEXCEPT: strExText(strValue), nExID(nExID)
{
    return;
}

MetaException::MetaException(const uint32_t nExID,  const char* pszValue)  _NOEXCEPT: strExText(pszValue), nExID(nExID)
{
    return;
}


MetaException::MetaException(const MetaException& exMetaException) _NOEXCEPT
{
    (*this) =exMetaException;
    return;
}

MetaException& MetaException::operator=(const MetaException& exMetaException) _NOEXCEPT
{
    this->nExID = exMetaException.nExID;
    this->strExText.assign(exMetaException.strExText);
    
    return (*this);
}


MetaException::~MetaException() _NOEXCEPT
{
    return;
}

const char* MetaException::what() const _NOEXCEPT
{
    return "Default Exception mode.";
}


void MetaException::verify(bool bCriteria, const u_int32_t nExID, const char* pszStringValue)
{
    if (!bCriteria)
    {
        std::stringstream strValue;
        
        strValue << "Exception at " <<  __FILE__ << "(" << __LINE__ << "):" << __PRETTY_FUNCTION__ << ":" << nExID << ":" << pszStringValue;
        
        throw new MetaException (nExID, strValue.str());
    }
}


const uint32_t MetaException::getExceptionID ()
{
    return (const uint32_t) nExID;
}

const char* MetaException::getExMessage ()
{
    return strExText.c_str();
}
