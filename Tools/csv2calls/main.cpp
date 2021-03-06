/*
 * Copyright (C) 2015 Christian Benjamin Ries
 * Website: www.christianbenjaminries.de
 * Source: https://github.com/cbries
 * License: MIT
 */

#include <QDir>
#include <QFile>
#include <QtMath>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>

#include <rgbhandling.h>

#define STDOUT QTextStream(stdout)

int main(int argc, char *argv[])
{
    QString infile;

    if(argc != 2)
    {
        STDOUT << QString("This application requires an input file which is") << endl;
        STDOUT << QString("generated by SceneEditor (see https://github.com/cbries/sceneeditor).") << endl;
        QFileInfo info(argv[0]);        
        STDOUT << QString("Usage: %0 CSV_INFILE").arg(info.baseName()) << endl;        
        return 1;
    }

    infile = argv[1];

    QFile f(infile);
    if(!f.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("Can not open file: %1").arg(infile);
        return 1;
    }
    
    QString cnt = f.readAll();
    f.close();

    QChar sep('\n');
    QStringList lines = cnt.split(sep, QString::KeepEmptyParts);
    if(lines.count() <= 0)
    {
        qDebug() << QString("File does not contain any line.");
        return 1;
    }  
    
    for(int i=0; i < lines.count(); ++i)
    {   
        QString line = lines[i];
    
        if(line.isEmpty()) { continue; }
    
        QStringList parts0 = line.split(',');
        QStringList parts1 = parts0[0].split('|');
        bool ok;
        int height = parts1[0].toInt(&ok);
        if(ok == false) { qDebug() << QString("Wrong format: %1").arg(parts1[0]); return 1; }
        int width = parts1[1].toInt(&ok);
        if(ok == false) { qDebug() << QString("Wrong format: %1").arg(parts1[1]); return 1; }
                
        int total = height * width;
        parts0.removeAt(0);
        if(total != parts0.count())
        {
            qDebug() << QString("Number of values does not match! Expected: %1, Got: %2")
                            .arg(total).arg(parts0.count());
            return 1;
        }
        
        QImage img(width, height, QImage::Format_ARGB32_Premultiplied);
        
        QFileInfo finfo0(infile);
        QString foutCppPath = QString("%1/Export").arg(finfo0.absolutePath());
        QFileInfo info(foutCppPath);
        if(info.exists() == false)
        {
            info.dir().mkpath(foutCppPath);
        }
        QString foutCpp = QString("%1/rgb%2.h").arg(foutCppPath).arg(i);
        QFile fcpp(foutCpp);
        if(!fcpp.open(QIODevice::Truncate | QIODevice::WriteOnly))
        {
            qDebug() << QString("Can not open CPP file for writing: %1").arg(foutCpp);
            continue;
        }
        
        QTextStream out(&fcpp);
        out << "void initGrid() { } \n";
        out << "void runGrid(){\n";
        
        for(int y=0; y < height; ++y)
        {
            for(int x=0; x < width; ++x)
            {
                int index = (y*width) + x;
                
                bool ok;
                
                auto bound = [=](float value) -> int {
                    return (int) (15.f/255.f * value);
                };
                
                QString p = parts0[index];
                QStringList rgb = p.split('|');
                int r = rgb[0].toUInt(&ok, 16);
                int g = rgb[1].toUInt(&ok, 16);
                int b = rgb[2].toUInt(&ok, 16);
                              
                out << QString("\tset_led_rgb(%1, %2, %3, %4, %5);")
                        .arg(x).arg(y)
                        .arg(bound(r)).arg(bound(g)).arg(bound(b));
                                
                img.setPixel(x, y, qRgb(r, g, b));
                
            }
            out << "\n";
        }
        
        out << "\tdelay(1000);\n";
        out << "}\n";
        out.flush();
        fcpp.close();
        
        QFileInfo finfo(infile);
        QString fout = QString("%1/Export/export%2.png").arg(finfo.absolutePath()).arg(i);
        bool res = img.save(fout);
        if(res == false)
        {
            qDebug() << QString("Storing of image failed: %1").arg(fout);
        }
    }
}
