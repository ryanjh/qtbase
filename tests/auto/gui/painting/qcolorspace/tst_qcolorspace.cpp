/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <qcolorspace.h>
#include <qimage.h>
#include <qimagereader.h>

#include <private/qcolorspace_p.h>

Q_DECLARE_METATYPE(QColorSpace::ColorSpaceId)
Q_DECLARE_METATYPE(QColorSpace::Gamut)
Q_DECLARE_METATYPE(QColorSpace::TransferFunction)

class tst_QColorSpace : public QObject
{
    Q_OBJECT

public:
    tst_QColorSpace();

private slots:
    void namedColorSpaces_data();
    void namedColorSpaces();

    void toIccProfile_data();
    void toIccProfile();

    void fromIccProfile();

    void imageConversion_data();
    void imageConversion();

    void loadImage();

    void gamut();
    void primariesXyz();
    void primaries2_data();
    void primaries2();
    void invalidPrimaries();
};

tst_QColorSpace::tst_QColorSpace()
{ }


void tst_QColorSpace::namedColorSpaces_data()
{
    QTest::addColumn<QColorSpace::ColorSpaceId>("colorSpaceId");
    QTest::addColumn<QColorSpace::Gamut>("gamutId");
    QTest::addColumn<QColorSpace::TransferFunction>("transferFunctionId");

    QTest::newRow("sRGB") << QColorSpace::SRgb
                          << QColorSpace::Gamut::SRgb
                          << QColorSpace::TransferFunction::SRgb;
    QTest::newRow("sRGB Linear") << QColorSpace::SRgbLinear
                                 << QColorSpace::Gamut::SRgb
                                 << QColorSpace::TransferFunction::Linear;
    QTest::newRow("Adobe RGB") << QColorSpace::AdobeRgb
                               << QColorSpace::Gamut::AdobeRgb
                               << QColorSpace::TransferFunction::Gamma;
    QTest::newRow("Display-P3") << QColorSpace::DisplayP3
                                << QColorSpace::Gamut::DciP3D65
                                << QColorSpace::TransferFunction::SRgb;
    QTest::newRow("ProPhoto RGB") << QColorSpace::ProPhotoRgb
                                  << QColorSpace::Gamut::ProPhotoRgb
                                  << QColorSpace::TransferFunction::ProPhotoRgb;
    QTest::newRow("BT.2020") << QColorSpace::Bt2020
                             << QColorSpace::Gamut::Bt2020
                             << QColorSpace::TransferFunction::Bt2020;
}

void tst_QColorSpace::namedColorSpaces()
{
    QFETCH(QColorSpace::ColorSpaceId, colorSpaceId);
    QFETCH(QColorSpace::Gamut, gamutId);
    QFETCH(QColorSpace::TransferFunction, transferFunctionId);

    QColorSpace colorSpace = colorSpaceId;

    QVERIFY(colorSpace.isValid());

    QCOMPARE(colorSpace.colorSpaceId(), colorSpaceId);
    QCOMPARE(colorSpace.gamut(), gamutId);
    QCOMPARE(colorSpace.transferFunction(), transferFunctionId);
}


void tst_QColorSpace::toIccProfile_data()
{
    namedColorSpaces_data();
}

void tst_QColorSpace::toIccProfile()
{
    QFETCH(QColorSpace::ColorSpaceId, colorSpaceId);
    QFETCH(QColorSpace::Gamut, gamutId);
    QFETCH(QColorSpace::TransferFunction, transferFunctionId);

    Q_UNUSED(gamutId);
    Q_UNUSED(transferFunctionId);

    QColorSpace colorSpace = colorSpaceId;
    QByteArray iccProfile = colorSpace.iccProfile();
    QVERIFY(!iccProfile.isEmpty());

    QColorSpace colorSpace2 = QColorSpace::fromIccProfile(iccProfile);
    QVERIFY(colorSpace2.isValid());

    QCOMPARE(colorSpace2, colorSpace);

    QByteArray iccProfile2 = colorSpace2.iccProfile();
    QVERIFY(!iccProfile2.isEmpty());

    QCOMPARE(iccProfile2, iccProfile);
}

void tst_QColorSpace::fromIccProfile()
{
    // Read the official sRGB ICCv2 profile:
    QString prefix = QFINDTESTDATA("resources/");
    QFile file(prefix + "sRGB2014.icc");
    file.open(QIODevice::ReadOnly);
    QByteArray iccProfile = file.readAll();
    QColorSpace stdSRgb = QColorSpace::fromIccProfile(iccProfile);
    QVERIFY(stdSRgb.isValid());

    QCOMPARE(stdSRgb.gamut(), QColorSpace::Gamut::SRgb);
    QCOMPARE(stdSRgb.transferFunction(), QColorSpace::TransferFunction::SRgb);
    QCOMPARE(stdSRgb.colorSpaceId(), QColorSpace::SRgb);

    QCOMPARE(stdSRgb, QColorSpace(QColorSpace::SRgb));
}

void tst_QColorSpace::imageConversion_data()
{
    QTest::addColumn<QColorSpace::ColorSpaceId>("fromColorSpace");
    QTest::addColumn<QColorSpace::ColorSpaceId>("toColorSpace");
    QTest::addColumn<int>("tolerance");

    QTest::newRow("sRGB -> Display-P3") << QColorSpace::SRgb << QColorSpace::DisplayP3 << 0;
    QTest::newRow("sRGB -> Adobe RGB") << QColorSpace::SRgb << QColorSpace::AdobeRgb << 2;
    QTest::newRow("Display-P3 -> sRGB") << QColorSpace::DisplayP3 << QColorSpace::SRgb << 0;
    QTest::newRow("Adobe RGB -> sRGB") << QColorSpace::AdobeRgb << QColorSpace::SRgb << 2;
    QTest::newRow("Display-P3 -> Adobe RGB") << QColorSpace::DisplayP3 << QColorSpace::AdobeRgb << 2;
    QTest::newRow("Display-P3 -> BT.2020") << QColorSpace::DisplayP3 << QColorSpace::Bt2020 << 4;
    QTest::newRow("sRGB -> sRGB Linear") << QColorSpace::SRgb << QColorSpace::SRgbLinear << 0;
}

void tst_QColorSpace::imageConversion()
{
    QFETCH(QColorSpace::ColorSpaceId, fromColorSpace);
    QFETCH(QColorSpace::ColorSpaceId, toColorSpace);
    QFETCH(int, tolerance);

    QImage testImage(256, 1, QImage::Format_RGB32);

    for (int i = 0; i < 256; ++i)
        testImage.setPixel(i, 0, qRgb(i, i, i));

    testImage.setColorSpace(fromColorSpace);
    QCOMPARE(testImage.colorSpace(), QColorSpace(fromColorSpace));

    testImage.convertToColorSpace(toColorSpace);
    QCOMPARE(testImage.colorSpace(), QColorSpace(toColorSpace));

    int lastRed = 0;
    int lastGreen = 0;
    int lastBlue = 0;
    for (int i = 0; i < 256; ++i) {
        QRgb p = testImage.pixel(i, 0);
        QVERIFY(qRed(p) >= lastRed);
        QVERIFY(qGreen(p) >= lastGreen);
        QVERIFY(qBlue(p) >= lastBlue);
        lastRed = qRed(p);
        lastGreen = qGreen(p);
        lastBlue = qBlue(p);
    }

    lastRed = 0;
    lastGreen = 0;
    lastBlue = 0;
    testImage.convertToColorSpace(fromColorSpace);
    QCOMPARE(testImage.colorSpace(), QColorSpace(fromColorSpace));
    for (int i = 0; i < 256; ++i) {
        QRgb p = testImage.pixel(i, 0);
        QVERIFY(qAbs(qRed(p) - qGreen(p)) <= tolerance);
        QVERIFY(qAbs(qRed(p) - qBlue(p)) <= tolerance);
        QVERIFY((lastRed   - qRed(p))   <= (tolerance / 2));
        QVERIFY((lastGreen - qGreen(p)) <= (tolerance / 2));
        QVERIFY((lastBlue  - qBlue(p))  <= (tolerance / 2));
        lastRed = qRed(p);
        lastGreen = qGreen(p);
        lastBlue = qBlue(p);
    }
}


void tst_QColorSpace::loadImage()
{
    QString prefix = QFINDTESTDATA("resources/");
    QImageReader reader(prefix + "ProPhoto.jpg");
    QImage image = reader.read();

    QVERIFY(!image.isNull());
    QVERIFY(image.colorSpace().isValid());
    QCOMPARE(image.colorSpace().colorSpaceId(), QColorSpace::ProPhotoRgb);
    QVERIFY(!image.colorSpace().iccProfile().isEmpty());

    QColorSpace defaultProPhotoRgb = QColorSpace::ProPhotoRgb;
    QVERIFY(!defaultProPhotoRgb.iccProfile().isEmpty());

    // Test the iccProfile getter returns the ICC profile from the image
    // which since we didn't write it, isn't identical to our defaults.
    QVERIFY(defaultProPhotoRgb.iccProfile() != image.colorSpace().iccProfile());

    QColorTransform transform = image.colorSpace().transformationToColorSpace(QColorSpace::SRgb);
    qreal maxRed = 0;
    qreal maxBlue = 0;
    qreal maxRed2 = 0;
    qreal maxBlue2 = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor p = image.pixelColor(x, y);
            maxRed = std::max(maxRed, p.redF());
            maxBlue = std::max(maxBlue, p.blueF());
            p = transform.map(p);
            maxRed2 = std::max(maxRed2, p.redF());
            maxBlue2 = std::max(maxBlue2, p.blueF());

        }
    }
    // ProPhotoRgb can be a lot more red and blue than SRgb can, so it will have lower values.
    QVERIFY(maxRed2 > maxRed);
    QVERIFY(maxBlue2 > maxBlue);
}

void tst_QColorSpace::gamut()
{
    QColor black = QColor::fromRgbF(0.0, 0.0, 0.0);
    QColor white = QColor::fromRgbF(1.0, 1.0, 1.0);
    QColor red = QColor::fromRgbF(1.0, 0.0, 0.0);
    QColor green = QColor::fromRgbF(0.0, 1.0, 0.0);
    QColor blue = QColor::fromRgbF(0.0, 0.0, 1.0);

    QColorTransform toAdobeRgb = QColorSpace(QColorSpace::SRgb).transformationToColorSpace(QColorSpace::AdobeRgb);

    QColor tblack = toAdobeRgb.map(black);
    QColor twhite = toAdobeRgb.map(white);
    QColor tred = toAdobeRgb.map(red);
    QColor tgreen = toAdobeRgb.map(green);
    QColor tblue = toAdobeRgb.map(blue);

    // Black is black
    QCOMPARE(tblack, black);

    // This white hasn't changed
    QCOMPARE(twhite, white);

    // Adobe's red and blue gamut corners are the same as sRGB's
    // So, a color in the red corner, will stay in the red corner
    // the same for blue, but not for green.
    QVERIFY(tred.greenF() < 0.001);
    QVERIFY(tred.blueF() < 0.001);
    QVERIFY(tblue.redF() < 0.001);
    QVERIFY(tblue.greenF() < 0.001);
    QVERIFY(tgreen.redF() > 0.2);
    QVERIFY(tgreen.blueF() > 0.2);
}

void tst_QColorSpace::primariesXyz()
{
    QColorSpace sRgb = QColorSpace::SRgb;
    QColorSpace adobeRgb = QColorSpace::AdobeRgb;
    QColorSpace displayP3 = QColorSpace::DisplayP3;
    QColorSpace proPhotoRgb = QColorSpace::ProPhotoRgb;
    QColorSpace bt2020 = QColorSpace::Bt2020;

    // Check if our calculated matrices, match the precalculated ones.
    QCOMPARE(sRgb.d_func()->toXyz, QColorMatrix::toXyzFromSRgb());
    QCOMPARE(adobeRgb.d_func()->toXyz, QColorMatrix::toXyzFromAdobeRgb());
    QCOMPARE(displayP3.d_func()->toXyz, QColorMatrix::toXyzFromDciP3D65());
    QCOMPARE(proPhotoRgb.d_func()->toXyz, QColorMatrix::toXyzFromProPhotoRgb());
    QCOMPARE(bt2020.d_func()->toXyz, QColorMatrix::toXyzFromBt2020());
}

void tst_QColorSpace::primaries2_data()
{
    QTest::addColumn<QColorSpace::Gamut>("gamut");

    QTest::newRow("sRGB") << QColorSpace::Gamut::SRgb;
    QTest::newRow("DCI-P3 (D65)") << QColorSpace::Gamut::DciP3D65;
    QTest::newRow("Adobe RGB (1998)") << QColorSpace::Gamut::AdobeRgb;
    QTest::newRow("ProPhoto RGB") << QColorSpace::Gamut::ProPhotoRgb;
    QTest::newRow("BT.2020") << QColorSpace::Gamut::Bt2020;
}

void tst_QColorSpace::primaries2()
{
    QFETCH(QColorSpace::Gamut, gamut);
    QColorSpacePrimaries primaries(gamut);

    QColorSpace original(gamut,  QColorSpace::TransferFunction::Linear);
    QColorSpace custom1(primaries.whitePoint, primaries.redPoint,
                        primaries.greenPoint, primaries.bluePoint, QColorSpace::TransferFunction::Linear);
    QCOMPARE(original, custom1);

    // A custom color swizzled color-space:
    QColorSpace custom2(primaries.whitePoint, primaries.bluePoint,
                        primaries.greenPoint, primaries.redPoint, QColorSpace::TransferFunction::Linear);

    QVERIFY(custom1 != custom2);
    QColor color1(255, 127, 63);
    QColor color2 = custom1.transformationToColorSpace(custom2).map(color1);
    QCOMPARE(color2.red(),   color1.blue());
    QCOMPARE(color2.green(), color1.green());
    QCOMPARE(color2.blue(),  color1.red());
    QCOMPARE(color2.alpha(), color1.alpha());
    QColor color3 = custom2.transformationToColorSpace(custom1).map(color2);
    QCOMPARE(color3.red(),   color1.red());
    QCOMPARE(color3.green(), color1.green());
    QCOMPARE(color3.blue(),  color1.blue());
    QCOMPARE(color3.alpha(), color1.alpha());
}

void tst_QColorSpace::invalidPrimaries()
{
    QColorSpace custom(QPointF(), QPointF(), QPointF(), QPointF(), QColorSpace::TransferFunction::Linear);
    QVERIFY(!custom.isValid());
    QCOMPARE(custom.colorSpaceId(), QColorSpace::Undefined);
}

QTEST_MAIN(tst_QColorSpace)
#include "tst_qcolorspace.moc"
