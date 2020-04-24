/************************************************************************

    chromadecoderconfigdialog.cpp

    ld-analyse - TBC output analysis
    Copyright (C) 2019 Simon Inns

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

#include "chromadecoderconfigdialog.h"
#include "ui_chromadecoderconfigdialog.h"

ChromaDecoderConfigDialog::ChromaDecoderConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChromaDecoderConfigDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    ui->chromaGainHorizontalSlider->setMinimum(0);
    ui->chromaGainHorizontalSlider->setMaximum(200);

    ui->thresholdHorizontalSlider->setMinimum(0);
    ui->thresholdHorizontalSlider->setMaximum(100);

    ui->cNRHorizontalSlider->setMinimum(0);
    ui->cNRHorizontalSlider->setMaximum(100);

    ui->yNRHorizontalSlider->setMinimum(0);
    ui->yNRHorizontalSlider->setMaximum(100);

    // Update the dialogue
    updateDialog();
}

ChromaDecoderConfigDialog::~ChromaDecoderConfigDialog()
{
    delete ui;
}

void ChromaDecoderConfigDialog::setConfiguration(bool _isSourcePal, const PalColour::Configuration &_palConfiguration,
                                                 const Comb::Configuration &_ntscConfiguration)
{
    isSourcePal = _isSourcePal;
    palConfiguration = _palConfiguration;
    ntscConfiguration = _ntscConfiguration;

    palConfiguration.chromaGain = qBound(0.0, palConfiguration.chromaGain, 2.0);
    palConfiguration.transformThreshold = qBound(0.0, palConfiguration.transformThreshold, 1.0);
    ntscConfiguration.cNRLevel = qBound(0.0, ntscConfiguration.cNRLevel, 10.0);
    ntscConfiguration.yNRLevel = qBound(0.0, ntscConfiguration.yNRLevel, 10.0);

    // ld-analyse only supports 2D filters at the moment
    if (palConfiguration.chromaFilter == PalColour::transform3DFilter) {
        palConfiguration.chromaFilter = PalColour::transform2DFilter;
    }
    ntscConfiguration.use3D = false;

    // For settings that both decoders share, the PAL default takes precedence
    ntscConfiguration.chromaGain = palConfiguration.chromaGain;

    // Select the tab corresponding to the current standard automatically
    if (isSourcePal) {
        ui->standardTabs->setCurrentWidget(ui->palTab);
    } else {
        ui->standardTabs->setCurrentWidget(ui->ntscTab);
    }

    updateDialog();
    emit chromaDecoderConfigChanged();
}

const PalColour::Configuration &ChromaDecoderConfigDialog::getPalConfiguration()
{
    return palConfiguration;
}

const Comb::Configuration &ChromaDecoderConfigDialog::getNtscConfiguration()
{
    return ntscConfiguration;
}

void ChromaDecoderConfigDialog::updateDialog()
{
    // Shared settings

    ui->chromaGainHorizontalSlider->setEnabled(true);
    ui->chromaGainHorizontalSlider->setValue(static_cast<qint32>(palConfiguration.chromaGain * 100));

    ui->chromaGainValueLabel->setEnabled(true);
    ui->chromaGainValueLabel->setText(QString::number(palConfiguration.chromaGain, 'f', 2));

    // PAL settings

    const bool isTransform2D = (palConfiguration.chromaFilter == PalColour::transform2DFilter);
    const bool isTransform = isTransform2D;
    ui->twoDeeTransformCheckBox->setEnabled(isSourcePal);
    ui->twoDeeTransformCheckBox->setChecked(isTransform2D);

    const bool isThresholdMode = (palConfiguration.transformMode == TransformPal::thresholdMode);
    ui->thresholdModeCheckBox->setEnabled(isSourcePal && isTransform);
    ui->thresholdModeCheckBox->setChecked(isThresholdMode);

    ui->thresholdLabel->setEnabled(isSourcePal && isTransform && isThresholdMode);

    ui->thresholdHorizontalSlider->setEnabled(isSourcePal && isTransform && isThresholdMode);
    ui->thresholdHorizontalSlider->setValue(static_cast<qint32>(palConfiguration.transformThreshold * 100));

    ui->thresholdValueLabel->setEnabled(isSourcePal && isTransform && isThresholdMode);
    ui->thresholdValueLabel->setText(QString::number(palConfiguration.transformThreshold, 'f', 2));

    ui->showFFTsCheckBox->setEnabled(isSourcePal && isTransform);
    ui->showFFTsCheckBox->setChecked(palConfiguration.showFFTs);

    ui->simplePALCheckBox->setEnabled(isSourcePal && isTransform);
    ui->simplePALCheckBox->setChecked(palConfiguration.simplePAL);

    // NTSC settings

    const bool isSourceNtsc = !isSourcePal;

    ui->whitePoint75CheckBox->setEnabled(isSourceNtsc);
    ui->whitePoint75CheckBox->setChecked(ntscConfiguration.whitePoint75);

    ui->colorLpfHqCheckBox->setEnabled(isSourceNtsc);
    ui->colorLpfHqCheckBox->setChecked(ntscConfiguration.colorlpf_hq);

    ui->cNRLabel->setEnabled(isSourceNtsc);

    ui->cNRHorizontalSlider->setEnabled(isSourceNtsc);
    ui->cNRHorizontalSlider->setValue(static_cast<qint32>(ntscConfiguration.cNRLevel * 10));

    ui->cNRValueLabel->setEnabled(isSourceNtsc);
    ui->cNRValueLabel->setText(QString::number(ntscConfiguration.cNRLevel, 'f', 1) + " IRE");

    ui->yNRLabel->setEnabled(isSourceNtsc);

    ui->yNRHorizontalSlider->setEnabled(isSourceNtsc);
    ui->yNRHorizontalSlider->setValue(static_cast<qint32>(ntscConfiguration.yNRLevel * 10));

    ui->yNRValueLabel->setEnabled(isSourceNtsc);
    ui->yNRValueLabel->setText(QString::number(ntscConfiguration.yNRLevel, 'f', 1) + " IRE");
}

// XXX Select the right tab when first opened

// Methods to handle changes to the dialogue

void ChromaDecoderConfigDialog::on_chromaGainHorizontalSlider_valueChanged(int value)
{
    palConfiguration.chromaGain = static_cast<double>(value) / 100;
    ntscConfiguration.chromaGain = palConfiguration.chromaGain;
    ui->chromaGainValueLabel->setText(QString::number(palConfiguration.chromaGain, 'f', 2));
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_twoDeeTransformCheckBox_clicked()
{
    if (ui->twoDeeTransformCheckBox->isChecked()) {
        palConfiguration.chromaFilter = PalColour::transform2DFilter;
    } else {
        palConfiguration.chromaFilter = PalColour::palColourFilter;
    }
    updateDialog();
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_thresholdModeCheckBox_clicked()
{
    if (ui->thresholdModeCheckBox->isChecked()) {
        palConfiguration.transformMode = TransformPal::thresholdMode;
    } else {
        palConfiguration.transformMode = TransformPal::levelMode;
    }
    updateDialog();
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_thresholdHorizontalSlider_valueChanged(int value)
{
    palConfiguration.transformThreshold = static_cast<double>(value) / 100;
    ui->thresholdValueLabel->setText(QString::number(palConfiguration.transformThreshold, 'f', 2));
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_showFFTsCheckBox_clicked()
{
    palConfiguration.showFFTs = ui->showFFTsCheckBox->isChecked();
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_simplePALCheckBox_clicked()
{
    palConfiguration.simplePAL = ui->simplePALCheckBox->isChecked();
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_whitePoint75CheckBox_clicked()
{
    ntscConfiguration.whitePoint75 = ui->whitePoint75CheckBox->isChecked();
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_colorLpfHqCheckBox_clicked()
{
    ntscConfiguration.colorlpf_hq = ui->colorLpfHqCheckBox->isChecked();
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_cNRHorizontalSlider_valueChanged(int value)
{
    ntscConfiguration.cNRLevel = static_cast<double>(value) / 10;
    ui->cNRValueLabel->setText(QString::number(ntscConfiguration.cNRLevel, 'f', 1) + " IRE");
    emit chromaDecoderConfigChanged();
}

void ChromaDecoderConfigDialog::on_yNRHorizontalSlider_valueChanged(int value)
{
    ntscConfiguration.yNRLevel = static_cast<double>(value) / 10;
    ui->yNRValueLabel->setText(QString::number(ntscConfiguration.yNRLevel, 'f', 1) + " IRE");
    emit chromaDecoderConfigChanged();
}
