  /************************************************************************

    vbiencoder.cpp

    ld-decode-tools TBC library
    Copyright (C) 2020 Simon Inns

    This file is part of ld-decode-tools.

    ld-decode-tools is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

************************************************************************/

#include "vbiencoder.h"

VbiEncoder::VbiEncoder()
{
    // Default setting for VBI data
    vbi.type = VbiDiscTypes::unknownDiscType;
    vbi.userCode = "";
    vbi.picNo = -1;
    vbi.chNo = -1;
    vbi.clvHr = -1;
    vbi.clvMin = -1;
    vbi.clvSec = -1;
    vbi.clvPicNo = -1;
    vbi.soundMode = VbiSoundModes::futureUse;
    vbi.soundModeAm2 = VbiSoundModes::futureUse;

    vbi.leadIn = false;
    vbi.leadOut = false;
    vbi.picStop = false;
    vbi.cx = false;
    vbi.size = false;
    vbi.side = false;
    vbi.teletext = false;
    vbi.dump = false;
    vbi.fm = false;
    vbi.digital = false;
    vbi.parity = false;
    vbi.copyAm2 = false;
    vbi.standardAm2 = false;

    // Reset the current VBI data
    vbi16_1 = 0;
    vbi17_1 = 0;
    vbi18_1 = 0;
    vbi16_2 = 0;
    vbi17_2 = 0;
    vbi18_2 = 0;
}

// Get methods

// Get the raw VBI line data for both fields
void VbiEncoder::getRawVbiData(qint32& _vbi16_1, qint32& _vbi17_1, qint32& _vbi18_1,
           qint32& _vbi16_2, qint32& _vbi17_2, qint32& _vbi18_2)
{
    // Pass back the result
    qDebug() << "Encoded vbi (line 1): 16 =" << QString("0x%1").arg(vbi16_1, 6, 16, QLatin1Char('0'))
             << "17 =" << QString("0x%1").arg(vbi17_1, 6, 16, QLatin1Char('0'))
             << "18 =" << QString("0x%1").arg(vbi18_1, 6, 16, QLatin1Char('0'));
    qDebug() << "Encoded vbi (line 2): 16 =" << QString("0x%1").arg(vbi16_2, 6, 16, QLatin1Char('0'))
             << "17 =" << QString("0x%1").arg(vbi17_2, 6, 16, QLatin1Char('0'))
             << "18 =" << QString("0x%1").arg(vbi18_2, 6, 16, QLatin1Char('0'));

    _vbi16_1 = vbi16_1;
    _vbi17_1 = vbi17_1;
    _vbi18_1 = vbi18_1;

    _vbi16_2 = vbi16_2;
    _vbi17_2 = vbi17_2;
    _vbi18_2 = vbi18_2;
}

// Get the VBI data
VbiEncoder::Vbi VbiEncoder::getVbiData()
{
    return vbi;
}

// Set methods

// Set the VBI data
void VbiEncoder::setVbiData(Vbi _vbi)
{
    // Reset the current VBI data
    vbi16_1 = 0;
    vbi17_1 = 0;
    vbi18_1 = 0;
    vbi16_2 = 0;
    vbi17_2 = 0;
    vbi18_2 = 0;

    vbi = _vbi;

    // Encode the VBI data
    qDebug() << "Encoding the VBI data...";
}
