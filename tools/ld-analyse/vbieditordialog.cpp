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

    // Initial state is uneditable
    editable(false);
}

VbiEditorDialog::~VbiEditorDialog()
{
    delete ui;
}

// Update the dialogue based on the VBI data
void VbiEditorDialog::updateDialog(LdDecodeMetaData::Vbi firstField, LdDecodeMetaData::Vbi secondField)
{
    // Display the raw VBI data
    ui->vbidata_firstField_data1_lineEdit->setText(QString::number(firstField.vbiData[0]));
    ui->vbidata_firstField_data2_lineEdit->setText(QString::number(firstField.vbiData[1]));
    ui->vbidata_firstField_data3_lineEdit->setText(QString::number(firstField.vbiData[2]));

    ui->vbidata_secondField_data1_lineEdit->setText(QString::number(secondField.vbiData[0]));
    ui->vbidata_secondField_data2_lineEdit->setText(QString::number(secondField.vbiData[1]));
    ui->vbidata_secondField_data3_lineEdit->setText(QString::number(secondField.vbiData[2]));
}

// Make the dialogue editable
void VbiEditorDialog::editable(bool state)
{
    // VBI data
    ui->vbidata_firstField_data1_lineEdit->setReadOnly(!state);
    ui->vbidata_firstField_data2_lineEdit->setReadOnly(!state);
    ui->vbidata_firstField_data3_lineEdit->setReadOnly(!state);

    ui->vbidata_secondField_data1_lineEdit->setReadOnly(!state);
    ui->vbidata_secondField_data2_lineEdit->setReadOnly(!state);
    ui->vbidata_secondField_data3_lineEdit->setReadOnly(!state);

    // Frame info
    ui->frameInfo_discType_comboBox->setEnabled(state);
    ui->frameInfo_frameNumber_spinBox->setEnabled(state);
    ui->frameInfo_timeCode_timeEdit->setEnabled(state);
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
    ui->original_parity_comboBox->setEnabled(state);

    // Programme status - Amendment 2
    ui->amendment2_cx_comboBox->setEnabled(state);
    ui->amendment2_discSize_comboBox->setEnabled(state);
    ui->amendment2_discSide_comboBox->setEnabled(state);
    ui->amendment2_teletext_comboBox->setEnabled(state);
    ui->amendment2_copy_comboBox->setEnabled(state);
    ui->amendment2_stdVideo_comboBox->setEnabled(state);
    ui->amendment2_soundMode_comboBox->setEnabled(state);
}

