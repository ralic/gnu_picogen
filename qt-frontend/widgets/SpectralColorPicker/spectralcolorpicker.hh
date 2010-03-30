//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2010  Sebastian Mach (*1983)
// * mail: phresnel/at/gmail/dot/com
// * http://phresnel.org
// * http://picogen.org
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef SPECTRALCOLORPICKER_HH
#define SPECTRALCOLORPICKER_HH

#include <QWidget>
#include "redshift/include/setup.hh"


namespace Ui {
    class SpectralColorPicker;
}


class SpectralSlider;
class SpectralColorPicker : public QWidget {
    Q_OBJECT
public:
        SpectralColorPicker(QWidget *parent = 0);
        ~SpectralColorPicker();

        typedef redshift::ReferenceSpectrum Spectrum;

protected:
        void changeEvent(QEvent *e);

private slots:
        void on_minAmp_editingFinished();
        void on_maxAmp_editingFinished();
        void on_importRawDataButton_pressed();
        void on_lockSampleCount_toggled(bool checked);
        void on_sampleCount_valueChanged(int );
        void valueChanged (double amplitude, double wavelength);

signals:
        void maxAmplitudeChanged (double amp);
        void minAmplitudeChanged (double amp);

private:
        Ui::SpectralColorPicker *ui;
        QVector<SpectralSlider*> sliders;

        void removeSpectralSliders();
        void addSpectralSliders(unsigned int i);
        Spectrum spectrumFromSliders () const;

        void updatePixmap();
};

#endif // SPECTRALCOLORPICKER_HH