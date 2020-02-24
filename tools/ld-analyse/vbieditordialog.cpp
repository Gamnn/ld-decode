/************************************************************************

    vbieditordialog.cpp

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

#include "vbieditordialog.h"
#include "ui_vbieditordialog.h"

VbiEditorDialog::VbiEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VbiEditorDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    initialise();
}

VbiEditorDialog::~VbiEditorDialog()
{
    delete ui;
}

// Update the dialogue based on the VBI data
void VbiEditorDialog::updateDialog(LdDecodeMetaData::Vbi firstField, LdDecodeMetaData::Vbi secondField)
{
    // Display the raw VBI data
    ui->vbidata_firstField_data1_lineEdit->setText(QString("0x%1").arg(firstField.vbiData[0], 6, 16, QLatin1Char('0')));
    ui->vbidata_firstField_data2_lineEdit->setText(QString("0x%1").arg(firstField.vbiData[1], 6, 16, QLatin1Char('0')));
    ui->vbidata_firstField_data3_lineEdit->setText(QString("0x%1").arg(firstField.vbiData[2], 6, 16, QLatin1Char('0')));

    ui->vbidata_secondField_data1_lineEdit->setText(QString("0x%1").arg(secondField.vbiData[0], 6, 16, QLatin1Char('0')));
    ui->vbidata_secondField_data2_lineEdit->setText(QString("0x%1").arg(secondField.vbiData[1], 6, 16, QLatin1Char('0')));
    ui->vbidata_secondField_data3_lineEdit->setText(QString("0x%1").arg(secondField.vbiData[2], 6, 16, QLatin1Char('0')));

    // Decode the VBI data for the frame
    currentFrameVbi = vbiDecoder.decodeFrame(firstField.vbiData[0], firstField.vbiData[1], firstField.vbiData[2],
            secondField.vbiData[0], secondField.vbiData[1], secondField.vbiData[2]);

    // Update the fields
    updateFields();

    // Disable the reset and apply buttons
    ui->apply_pushButton->setEnabled(false);
    ui->reset_pushButton->setEnabled(false);

    // Set the modified flag
    vbiModified = false;
}

void VbiEditorDialog::updateFields()
{
    // Update frame number and timecode
    if (currentFrameVbi.picNo != -1) {
        // Valid CAV frame number
        ui->frameInfo_frameNumber_spinBox->setValue(currentFrameVbi.picNo);
        ui->frameInfo_timecode_timeEdit->setTime(QTime(0, 0, 0));
        ui->frameInfo_clvPicNo_spinBox->setValue(0);
    } else if (currentFrameVbi.clvPicNo == -1) {
        // Early CLV with minute only timecode
        ui->frameInfo_timecode_timeEdit->setTime(QTime(0, currentFrameVbi.clvMin, 0));
        ui->frameInfo_clvPicNo_spinBox->setValue(0);
        ui->frameInfo_frameNumber_spinBox->setValue(0);
    } else if (currentFrameVbi.clvMin == -1) {
        // No timecode or frame number
        ui->frameInfo_timecode_timeEdit->setTime(QTime(0, 0, 0));
        ui->frameInfo_clvPicNo_spinBox->setValue(0);
        ui->frameInfo_frameNumber_spinBox->setValue(0);
    } else {
        // CAV timecode (full)
        ui->frameInfo_timecode_timeEdit->setTime(QTime(currentFrameVbi.clvHr, currentFrameVbi.clvMin, currentFrameVbi.clvSec));
        ui->frameInfo_clvPicNo_spinBox->setValue(currentFrameVbi.clvPicNo);
        ui->frameInfo_frameNumber_spinBox->setValue(0);
    }

    // Update the chapter number
    if (currentFrameVbi.chNo != -1) {
        ui->frameInfo_chapter_spinBox->setValue(currentFrameVbi.chNo);
    } else {
        ui->frameInfo_chapter_spinBox->setValue(0);
    }
    if (!currentFrameVbi.leadIn && !currentFrameVbi.leadOut) ui->frameInfo_chapter_spinBox->setEnabled(true);
    else ui->frameInfo_chapter_spinBox->setEnabled(false);

    // Update the frame type (lead-in, lead-out or visible)
    // and the usercode
    if (currentFrameVbi.leadIn) {
        ui->frameInfo_type_comboBox->setCurrentIndex(ui->frameInfo_type_comboBox->findData(1));

        // Lead-in frames can have a user-code
        ui->frameInfo_userCode_lineEdit->setEnabled(true);
        ui->frameInfo_userCode_lineEdit->setText(currentFrameVbi.userCode);

        // Disc size and side is not valid for lead-in
        ui->original_discSide_comboBox->setEnabled(false);
        ui->original_discSize_comboBox->setEnabled(false);
        ui->amendment2_discSide_comboBox->setEnabled(false);
        ui->amendment2_discSize_comboBox->setEnabled(false);

        // Frame number and timecode not valid for lead-in
        ui->frameInfo_frameNumber_spinBox->setEnabled(false);
        ui->frameInfo_timecode_timeEdit->setEnabled(false);
        ui->frameInfo_clvPicNo_spinBox->setEnabled(false);
    } else if (currentFrameVbi.leadOut) {
        ui->frameInfo_type_comboBox->setCurrentIndex(ui->frameInfo_type_comboBox->findData(2));

        // Lead-out frames can have a user-code
        ui->frameInfo_userCode_lineEdit->setEnabled(true);
        ui->frameInfo_userCode_lineEdit->setText(currentFrameVbi.userCode);

        // Disc size and side is not valid for lead-out
        ui->original_discSide_comboBox->setEnabled(false);
        ui->original_discSize_comboBox->setEnabled(false);
        ui->amendment2_discSide_comboBox->setEnabled(false);
        ui->amendment2_discSize_comboBox->setEnabled(false);

        // Frame number and timecode not valid for lead-in
        ui->frameInfo_frameNumber_spinBox->setEnabled(false);
        ui->frameInfo_timecode_timeEdit->setEnabled(false);
        ui->frameInfo_clvPicNo_spinBox->setEnabled(false);
    } else {
        ui->frameInfo_type_comboBox->setCurrentIndex(ui->frameInfo_type_comboBox->findData(0));

        // Visible frames cannot have a user-code
        ui->frameInfo_userCode_lineEdit->setEnabled(false);
        ui->frameInfo_userCode_lineEdit->setText("");

        // Disc size and side is valid for visible
        ui->original_discSide_comboBox->setEnabled(true);
        ui->original_discSize_comboBox->setEnabled(true);
        ui->amendment2_discSide_comboBox->setEnabled(true);
        ui->amendment2_discSize_comboBox->setEnabled(true);
    }

    // Update the disc type
    if (currentFrameVbi.type == VbiDecoder::VbiDiscTypes::unknownDiscType) {
        ui->frameInfo_discType_comboBox->setCurrentIndex(ui->frameInfo_discType_comboBox->findData(0));
        ui->frameInfo_frameNumber_spinBox->setEnabled(false);
        ui->frameInfo_timecode_timeEdit->setEnabled(false);
        ui->frameInfo_clvPicNo_spinBox->setEnabled(false);
    } else if (currentFrameVbi.type == VbiDecoder::VbiDiscTypes::cav) {
        ui->frameInfo_discType_comboBox->setCurrentIndex(ui->frameInfo_discType_comboBox->findData(1));
        ui->frameInfo_frameNumber_spinBox->setEnabled(true);
        ui->frameInfo_timecode_timeEdit->setEnabled(false);
        ui->frameInfo_clvPicNo_spinBox->setEnabled(false);
    } else {
        ui->frameInfo_discType_comboBox->setCurrentIndex(ui->frameInfo_discType_comboBox->findData(2));
        ui->frameInfo_frameNumber_spinBox->setEnabled(false);
        ui->frameInfo_timecode_timeEdit->setEnabled(true);
        ui->frameInfo_clvPicNo_spinBox->setEnabled(true);
    }

    // Stop code
    if (currentFrameVbi.type == VbiDecoder::VbiDiscTypes::cav && (!currentFrameVbi.leadIn && !currentFrameVbi.leadOut)) ui->frameInfo_stopCode_comboBox->setEnabled(true);
    else ui->frameInfo_stopCode_comboBox->setEnabled(false);
    if (currentFrameVbi.picStop) ui->frameInfo_stopCode_comboBox->setCurrentIndex(ui->frameInfo_stopCode_comboBox->findData(1));
    else ui->frameInfo_stopCode_comboBox->setCurrentIndex(ui->frameInfo_stopCode_comboBox->findData(0));

    // Programme status - original and amendment2
    // CX
    if (currentFrameVbi.cx) {
        ui->original_cx_comboBox->setCurrentIndex(ui->original_cx_comboBox->findData(1));
        ui->amendment2_cx_comboBox->setCurrentIndex(ui->amendment2_cx_comboBox->findData(1));
    } else {
        ui->original_cx_comboBox->setCurrentIndex(ui->original_cx_comboBox->findData(0));
        ui->amendment2_cx_comboBox->setCurrentIndex(ui->amendment2_cx_comboBox->findData(0));
    }

    // Side
    if (currentFrameVbi.side) {
        ui->original_discSide_comboBox->setCurrentIndex(ui->original_discSide_comboBox->findData(0));
        ui->amendment2_discSide_comboBox->setCurrentIndex(ui->amendment2_discSide_comboBox->findData(0));
    } else {
        ui->original_discSide_comboBox->setCurrentIndex(ui->original_discSide_comboBox->findData(1));
        ui->amendment2_discSide_comboBox->setCurrentIndex(ui->amendment2_discSide_comboBox->findData(1));
    }

    // Size
    if (currentFrameVbi.size) {
        ui->original_discSize_comboBox->setCurrentIndex(ui->original_discSize_comboBox->findData(0));
        ui->amendment2_discSize_comboBox->setCurrentIndex(ui->amendment2_discSize_comboBox->findData(0));
    } else {
        ui->original_discSize_comboBox->setCurrentIndex(ui->original_discSize_comboBox->findData(1));
        ui->amendment2_discSize_comboBox->setCurrentIndex(ui->amendment2_discSize_comboBox->findData(1));
    }

    // Teletext
    if (currentFrameVbi.teletext) {
        ui->original_teletext_comboBox->setCurrentIndex(ui->original_teletext_comboBox->findData(1));
        ui->amendment2_teletext_comboBox->setCurrentIndex(ui->amendment2_teletext_comboBox->findData(1));
    } else {
        ui->original_teletext_comboBox->setCurrentIndex(ui->original_teletext_comboBox->findData(0));
        ui->amendment2_teletext_comboBox->setCurrentIndex(ui->amendment2_teletext_comboBox->findData(0));
    }

    // Programme dump (original only)
    if (currentFrameVbi.dump) {
        ui->original_progDump_comboBox->setCurrentIndex(ui->original_progDump_comboBox->findData(1));
    } else {
        ui->original_progDump_comboBox->setCurrentIndex(ui->original_progDump_comboBox->findData(0));
    }

    // FM/FM (original only)
    if (currentFrameVbi.fm) {
        ui->original_fmFm_comboBox->setCurrentIndex(ui->original_fmFm_comboBox->findData(1));
    } else {
        ui->original_fmFm_comboBox->setCurrentIndex(ui->original_fmFm_comboBox->findData(0));
    }

    // Digital sound (original only)
    if (currentFrameVbi.digital) {
        ui->original_digital_comboBox->setCurrentIndex(ui->original_digital_comboBox->findData(1));
    } else {
        ui->original_digital_comboBox->setCurrentIndex(ui->original_digital_comboBox->findData(0));
    }

    // Parity (original only)
    if (currentFrameVbi.parity) {
        ui->original_parity_label->setText("Valid");
    } else {
        ui->original_parity_label->setText("Invalid");
    }

    // Copy allowed (amendment2 only)
    if (currentFrameVbi.copyAm2) {
        ui->amendment2_copy_comboBox->setCurrentIndex(ui->amendment2_copy_comboBox->findData(1));
    } else {
        ui->amendment2_copy_comboBox->setCurrentIndex(ui->amendment2_copy_comboBox->findData(0));
    }

    // Standard video (amendment2 only)
    if (currentFrameVbi.copyAm2) {
        ui->amendment2_stdVideo_comboBox->setCurrentIndex(ui->amendment2_stdVideo_comboBox->findData(1));
    } else {
        ui->amendment2_stdVideo_comboBox->setCurrentIndex(ui->amendment2_stdVideo_comboBox->findData(0));
    }

    // Sound mode (original)
    switch(currentFrameVbi.soundMode) {
        case VbiDecoder::VbiSoundModes::stereo:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(0));
        break;

        case VbiDecoder::VbiSoundModes::mono:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(1));
        break;

        case VbiDecoder::VbiSoundModes::audioSubCarriersOff:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(2));
        break;

        case VbiDecoder::VbiSoundModes::bilingual:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(3));
        break;

        case VbiDecoder::VbiSoundModes::stereo_stereo:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(4));
        break;

        case VbiDecoder::VbiSoundModes::stereo_bilingual:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(5));
        break;

        case VbiDecoder::VbiSoundModes::crossChannelStereo:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(6));
        break;

        case VbiDecoder::VbiSoundModes::bilingual_bilingual:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(7));
        break;

        case VbiDecoder::VbiSoundModes::mono_dump:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(8));
        break;

        case VbiDecoder::VbiSoundModes::stereo_dump:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(9));
        break;

        case VbiDecoder::VbiSoundModes::bilingual_dump:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(10));
        break;

        case VbiDecoder::VbiSoundModes::futureUse:
        ui->original_soundMode_comboBox->setCurrentIndex(ui->original_soundMode_comboBox->findData(11));
        break;
    }

    // Sound mode (amendment2)
    switch(currentFrameVbi.soundModeAm2) {
        case VbiDecoder::VbiSoundModes::stereo:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(0));
        break;

        case VbiDecoder::VbiSoundModes::mono:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(1));
        break;

        case VbiDecoder::VbiSoundModes::audioSubCarriersOff:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(2));
        break;

        case VbiDecoder::VbiSoundModes::bilingual:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(3));
        break;

        case VbiDecoder::VbiSoundModes::stereo_stereo:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(4));
        break;

        case VbiDecoder::VbiSoundModes::stereo_bilingual:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(5));
        break;

        case VbiDecoder::VbiSoundModes::crossChannelStereo:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(6));
        break;

        case VbiDecoder::VbiSoundModes::bilingual_bilingual:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(7));
        break;

        case VbiDecoder::VbiSoundModes::mono_dump:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(8));
        break;

        case VbiDecoder::VbiSoundModes::stereo_dump:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(9));
        break;

        case VbiDecoder::VbiSoundModes::bilingual_dump:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(10));
        break;

        case VbiDecoder::VbiSoundModes::futureUse:
        ui->amendment2_soundMode_comboBox->setCurrentIndex(ui->amendment2_soundMode_comboBox->findData(11));
        break;
    }
}

// Make the dialogue editable
void VbiEditorDialog::editable(bool state)
{
    // VBI data is never editable
    ui->vbidata_firstField_data1_lineEdit->setReadOnly(true);
    ui->vbidata_firstField_data2_lineEdit->setReadOnly(true);
    ui->vbidata_firstField_data3_lineEdit->setReadOnly(true);

    ui->vbidata_secondField_data1_lineEdit->setReadOnly(true);
    ui->vbidata_secondField_data2_lineEdit->setReadOnly(true);
    ui->vbidata_secondField_data3_lineEdit->setReadOnly(true);

    // Frame info
    ui->frameInfo_discType_comboBox->setEnabled(state);
    ui->frameInfo_frameNumber_spinBox->setEnabled(state);
    ui->frameInfo_timecode_timeEdit->setEnabled(state);
    ui->frameInfo_clvPicNo_spinBox->setEnabled(state);
    ui->frameInfo_chapter_spinBox->setEnabled(state);
    ui->frameInfo_type_comboBox->setEditable(state);
    ui->frameInfo_userCode_lineEdit->setEnabled(state);
    ui->frameInfo_stopCode_comboBox->setEditable(state);

    // Programme status - Original
    ui->original_cx_comboBox->setEnabled(state);
    ui->original_discSize_comboBox->setEnabled(state);
    ui->original_discSide_comboBox->setEnabled(state);
    ui->original_teletext_comboBox->setEnabled(state);
    ui->original_progDump_comboBox->setEnabled(state);
    ui->original_fmFm_comboBox->setEnabled(state);
    ui->original_digital_comboBox->setEnabled(state);
    ui->original_soundMode_comboBox->setEnabled(state);

    // Programme status - Amendment 2
    ui->amendment2_cx_comboBox->setEnabled(state);
    ui->amendment2_discSize_comboBox->setEnabled(state);
    ui->amendment2_discSide_comboBox->setEnabled(state);
    ui->amendment2_teletext_comboBox->setEnabled(state);
    ui->amendment2_copy_comboBox->setEnabled(state);
    ui->amendment2_stdVideo_comboBox->setEnabled(state);
    ui->amendment2_soundMode_comboBox->setEnabled(state);
}

// Initialise the dialogue
void VbiEditorDialog::initialise()
{
    // Set dialogue editable state
    editable(true);

    // Set up the comboboxes
    ui->frameInfo_discType_comboBox->addItem("Unknown", 0);
    ui->frameInfo_discType_comboBox->addItem("CAV", 1);
    ui->frameInfo_discType_comboBox->addItem("CLV", 2);
    ui->frameInfo_discType_comboBox->setEditable(false);

    ui->frameInfo_type_comboBox->addItem("Visible", 0);
    ui->frameInfo_type_comboBox->addItem("Lead-in", 1);
    ui->frameInfo_type_comboBox->addItem("Lead-out", 2);
    ui->frameInfo_type_comboBox->setEditable(false);

    ui->frameInfo_stopCode_comboBox->addItem("True", 1);
    ui->frameInfo_stopCode_comboBox->addItem("False", 0);
    ui->frameInfo_stopCode_comboBox->setEditable(false);

    ui->original_cx_comboBox->addItem("True", 1);
    ui->original_cx_comboBox->addItem("False", 0);
    ui->original_discSize_comboBox->addItem("12 inch", 0);
    ui->original_discSize_comboBox->addItem("8 inch", 1);
    ui->original_discSide_comboBox->addItem("1", 0);
    ui->original_discSide_comboBox->addItem("2", 1);
    ui->original_teletext_comboBox->addItem("True", 1);
    ui->original_teletext_comboBox->addItem("False", 0);
    ui->original_progDump_comboBox->addItem("True", 1);
    ui->original_progDump_comboBox->addItem("False", 0);
    ui->original_fmFm_comboBox->addItem("True", 1);
    ui->original_fmFm_comboBox->addItem("False", 0);
    ui->original_digital_comboBox->addItem("True", 1);
    ui->original_digital_comboBox->addItem("False", 0);

    ui->original_soundMode_comboBox->addItem("stereo", 0);
    ui->original_soundMode_comboBox->addItem("mono", 1);
    ui->original_soundMode_comboBox->addItem("audioSubCarriersOff", 2);
    ui->original_soundMode_comboBox->addItem("bilingual", 3);
    ui->original_soundMode_comboBox->addItem("stereo_stereo", 4);
    ui->original_soundMode_comboBox->addItem("stereo_bilingual", 5);
    ui->original_soundMode_comboBox->addItem("crossChannelStereo", 6);
    ui->original_soundMode_comboBox->addItem("bilingual_bilingual", 7);
    ui->original_soundMode_comboBox->addItem("mono_dump", 8);
    ui->original_soundMode_comboBox->addItem("stereo_dump", 9);
    ui->original_soundMode_comboBox->addItem("bilingual_dump", 10);
    ui->original_soundMode_comboBox->addItem("futureUse", 11);

    ui->amendment2_cx_comboBox->addItem("True", 1);
    ui->amendment2_cx_comboBox->addItem("False", 0);
    ui->amendment2_discSize_comboBox->addItem("12 inch", 0);
    ui->amendment2_discSize_comboBox->addItem("8 inch", 1);
    ui->amendment2_discSide_comboBox->addItem("1", 0);
    ui->amendment2_discSide_comboBox->addItem("2", 1);
    ui->amendment2_teletext_comboBox->addItem("True", 1);
    ui->amendment2_teletext_comboBox->addItem("False", 0);
    ui->amendment2_copy_comboBox->addItem("True", 1);
    ui->amendment2_copy_comboBox->addItem("False", 0);
    ui->amendment2_stdVideo_comboBox->addItem("True", 1);
    ui->amendment2_stdVideo_comboBox->addItem("False", 0);

    ui->amendment2_soundMode_comboBox->addItem("stereo", 0);
    ui->amendment2_soundMode_comboBox->addItem("mono", 1);
    ui->amendment2_soundMode_comboBox->addItem("bilingual", 3);
    ui->amendment2_soundMode_comboBox->addItem("mono_dump", 8);
    ui->amendment2_soundMode_comboBox->addItem("futureUse", 11);

    // Set up the spinboxes
    ui->frameInfo_frameNumber_spinBox->setMinimum(-1);
    ui->frameInfo_frameNumber_spinBox->setMaximum(79999);
    ui->frameInfo_chapter_spinBox->setMinimum(-1);
    ui->frameInfo_chapter_spinBox->setMaximum(99);

    // Misc setup
    ui->original_parity_label->setText("Unknown");
}









