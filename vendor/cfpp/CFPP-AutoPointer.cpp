/*******************************************************************************
 * Copyright (c) 2014, Jean-David Gadina - www.xs-labs.com / www.digidna.net
 * Distributed under the Boost Software License, Version 1.0.
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

/*!
 * @file        CFPP-AutoPointer.cpp
 * @copyright   (c) 2014 - Jean-David Gadina - www.xs-labs.com / www.digidna.net
 * @abstract    CoreFoundation++ auto-pointer for CF types
 */

#include <CF++.hpp>

namespace CF
{
    AutoPointer::AutoPointer( void ): _cfObject( NULL )
    {}
    
    AutoPointer::AutoPointer( const AutoPointer & value ): _cfObject( NULL )
    {
        if( value._cfObject != NULL )
        {
            CFRetain( value._cfObject );
        }
        
        this->_cfObject = value._cfObject;
    }
    
    AutoPointer::AutoPointer( CFTypeRef value ): _cfObject( NULL )
    {
        this->_cfObject = value;
    }
    
    #ifdef CFPP_HAS_CPP11
    AutoPointer::AutoPointer( AutoPointer && value )
    {
        this->_cfObject = value._cfObject;
        value._cfObject = nullptr;
    }
    #endif
    
    AutoPointer::~AutoPointer( void )
    {
        if( this->_cfObject != NULL )
        {
            CFRelease( this->_cfObject );
            
            this->_cfObject = NULL;
        }
    }
    
    AutoPointer & AutoPointer::operator = ( AutoPointer value )
    {
        swap( *( this ), value );
        
        return *( this );
    }
    
    AutoPointer & AutoPointer::operator = ( CFTypeRef value )
    {
        return operator =( AutoPointer( value ) );
    }
    
    CFTypeID AutoPointer::GetTypeID( void ) const
    {
        if( this->_cfObject == NULL )
        {
            return 0;
        }
        
        return CFGetTypeID( this->_cfObject );
    }
    
    CFTypeRef AutoPointer::GetCFObject( void ) const
    {
        return this->_cfObject;
    }
    
    void swap( AutoPointer & v1, AutoPointer & v2 )
    {
        using std::swap;
        
        swap( v1._cfObject, v2._cfObject );
    }
}
