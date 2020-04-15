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

    void updateDialog(LdDecodeMetaData::Vbi _firstField, LdDecodeMetaData::Vbi _secondField, bool _isSourcePal);

private slots:
    void on_frameInfo_discType_comboBox_currentIndexChanged(int);
    void on_frameInfo_frameNumber_spinBox_valueChanged(int);
    void on_frameInfo_timecode_timeEdit_userTimeChanged(const QTime &);
    void on_frameInfo_clvPicNo_spinBox_valueChanged(int);
    void on_frameInfo_chapter_spinBox_valueChanged(int);
    void on_frameInfo_type_comboBox_currentIndexChanged(int);
    void on_frameInfo_userCode_lineEdit_editingFinished();
    void on_frameInfo_stopCode_comboBox_currentIndexChanged(int);
    void on_original_cx_comboBox_currentIndexChanged(int);
    void on_original_discSize_comboBox_currentIndexChanged(int);
    void on_original_discSide_comboBox_currentIndexChanged(int);
    void on_original_teletext_comboBox_currentIndexChanged(int);
    void on_original_progDump_comboBox_currentIndexChanged(int);
    void on_original_fmFm_comboBox_currentIndexChanged(int);
    void on_original_digital_comboBox_currentIndexChanged(int);
    void on_original_soundMode_comboBox_currentIndexChanged(int);
    void on_amendment2_cx_comboBox_currentIndexChanged(int);
    void on_amendment2_discSize_comboBox_currentIndexChanged(int);
    void on_amendment2_discSide_comboBox_currentIndexChanged(int);
    void on_amendment2_teletext_comboBox_currentIndexChanged(int);
    void on_amendment2_copy_comboBox_currentIndexChanged(int);
    void on_amendment2_stdVideo_comboBox_currentIndexChanged(int);
    void on_amendment2_soundMode_comboBox_currentIndexChanged(int);
    void on_reset_pushButton_clicked();
    void on_apply_pushButton_clicked();

private:
    Ui::VbiEditorDialog *ui;

    VbiDecoder vbiDecoder;
    VbiDecoder::Vbi currentFrameVbi;
    LdDecodeMetaData::Vbi firstField;
    LdDecodeMetaData::Vbi secondField;
    bool isSourcePal;
    bool vbiModified;

    void updateFields();
    void editable(bool state);
    void initialise();
    void enableProgrammeStatus(bool state);
    void convertDialogueToVbi();
};

#endif // VBIEDITORDIALOG_H
