/************************************************************************

    comb.h

    ld-chroma-decoder - Colourisation filter for ld-decode
    Copyright (C) 2018 Chad Page
    Copyright (C) 2018-2019 Simon Inns

    This file is part of ld-decode-tools.

    ld-chroma-decoder is free software: you can redistribute it and/or
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

#ifndef COMB_H
#define COMB_H

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QtMath>

#include "lddecodemetadata.h"

#include "rgb.h"
#include "rgbframe.h"
#include "sourcefield.h"
#include "yiq.h"
#include "yiqbuffer.h"

class Comb
{
public:
    Comb();

    // Comb filter configuration parameters
    struct Configuration {
        double chromaGain = 1.0;
        bool colorlpf = true;
        bool colorlpf_hq = true;
        bool whitePoint75 = false;
        bool use3D = false;
        bool showOpticalFlowMap = false;

        qreal cNRLevel = 0.0;
        qreal yNRLevel = 1.0;
    };

    const Configuration &getConfiguration() const;
    void updateConfiguration(const LdDecodeMetaData::VideoParameters &videoParameters,
                             const Configuration &configuration);

    // Decode two fields to produce an interlaced frame.
    RGBFrame decodeFrame(const SourceField &firstField, const SourceField &secondField);

protected:

private:
    // Comb-filter configuration parameters
    bool configurationSet;
    Configuration configuration;
    LdDecodeMetaData::VideoParameters videoParameters;

    // IRE scaling
    qreal irescale;

    // Calculated frame height
    qint32 frameHeight;

    // Input frame buffer definitions
    struct PixelLine {
        qreal pixel[526][911]; // 526 is the maximum allowed field lines, 911 is the maximum field width
    };

    struct FrameBuffer {
        SourceVideo::Data rawbuffer;

        QVector<PixelLine> clpbuffer; // Unfiltered chroma for the current phase (can be I or Q)
        QVector<qreal> kValues;
        YiqBuffer yiqBuffer; // YIQ values for the frame

        qint32 firstFieldPhaseID; // The phase of the frame's first field
        qint32 secondFieldPhaseID; // The phase of the frame's second field
    };

    // Previous and next frame for 3D processing
    FrameBuffer previousFrameBuffer;

    inline qint32 GetFieldID(FrameBuffer *frameBuffer, qint32 lineNumber);
    inline bool GetLinePhase(FrameBuffer *frameBuffer, qint32 lineNumber);

    void split1D(FrameBuffer *frameBuffer);
    void split2D(FrameBuffer *frameBuffer);
    void split3D(FrameBuffer *currentFrame, FrameBuffer *previousFrame);

    void filterIQ(YiqBuffer &yiqBuffer);
    void splitIQ(FrameBuffer *frameBuffer);

    void doCNR(YiqBuffer &yiqBuffer);
    void doYNR(YiqBuffer &yiqBuffer);

    RGBFrame yiqToRgbFrame(const YiqBuffer &yiqBuffer);
    void overlayOpticalFlowMap(const FrameBuffer &frameBuffer, RGBFrame &rgbOutputFrame);
    void adjustY(FrameBuffer *frameBuffer, YiqBuffer &yiqBuffer);
};

#endif // COMB_H
