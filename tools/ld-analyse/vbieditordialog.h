/************************************************************************

    vbieditordialog.h

    ld-analyse - TBC output analysis
    Copyright (C) 2020 Simon Inns

    This file is part of ld-decode-tools.

    ld-analyse is free software: you can redistribute it and/or
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

#ifndef VBIEDITORDIALOG_H
#define VBIEDITORDIALOG_H

#include <QDialog>

#include "lddecodemetadata.h"
#include "vbidecoder.h"

namespace Ui {
class VbiEditorDialog;
}

class VbiEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VbiEditorDialog(QWidget *parent = nullptr);
    ~VbiEditorDialog();

    void updateDialog(LdDecodeMetaData::Vbi firstField, LdDecodeMetaData::Vbi secondField);

private:
    Ui::VbiEditorDialog *ui;

    VbiDecoder vbiDecoder;
    VbiDecoder::Vbi currentFrameVbi;
    bool vbiModified;

    void updateFields();
    void editable(bool state);
    void initialise();
};

#endif // VBIEDITORDIALOG_H
