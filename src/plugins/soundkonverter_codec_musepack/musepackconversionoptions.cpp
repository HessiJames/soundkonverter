/****************************************************************************************
 * soundKonverter - A frontend to various audio converters                              *
 * Copyright (c) 2010 - 2014 Daniel Faust <hessijames@gmail.com>                        *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/


#include "musepackcodecglobal.h"

#include "musepackconversionoptions.h"

MusePackConversionOptions::MusePackConversionOptions()
    : ConversionOptions()
{
    pluginName = global_plugin_name;
}

MusePackConversionOptions::~MusePackConversionOptions()
{}

bool MusePackConversionOptions::equals( ConversionOptions *_other )
{
    if( !_other || _other->pluginName!=pluginName )
        return false;

    MusePackConversionOptions *other = dynamic_cast<MusePackConversionOptions*>(_other);

    if( data.preset==other->data.preset && data.preset==Data::UserDefined )
    {
        return ConversionOptions::equals(_other);
    }
    else if( data.preset==other->data.preset )
    {
        return ( equalsBasics(_other) && equalsFilters(_other) );
    }
    else
    {
        return false;
    }
}

QDomElement MusePackConversionOptions::toXml( QDomDocument document )
{
    QDomElement conversionOptions = ConversionOptions::toXml( document );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = document.createElement("data");
    data.setAttribute("preset",MusePackConversionOptions::data.preset);
    encodingOptions.appendChild(data);

    return conversionOptions;
}

bool MusePackConversionOptions::fromXml( QDomElement conversionOptions, QList<QDomElement> *filterOptionsElements )
{
    ConversionOptions::fromXml( conversionOptions, filterOptionsElements );
    QDomElement encodingOptions = conversionOptions.elementsByTagName("encodingOptions").at(0).toElement();
    QDomElement data = encodingOptions.elementsByTagName("data").at(0).toElement();
    MusePackConversionOptions::data.preset = (Data::Preset)data.attribute("preset").toInt();
    return true;
}














