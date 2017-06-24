/****************************************************************************
 **
 ** Copyright (C) 2008-2014 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the jom project on Trolltech Labs.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 or 3.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 ** http://www.gnu.org/copyleft/gpl.html.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#ifndef MAKEFILELINEREADER_H
#define MAKEFILELINEREADER_H

#include <QtCore/QFile>
#include <QtCore/QTextStream>

namespace NMakeFile {

class MakefileLineReader {
public:
    MakefileLineReader(const QString& filename);
    ~MakefileLineReader();

    bool open();
    void close();
    QString readLine(bool bInlineFileMode);
    QString fileName() const { return m_file.fileName(); }
    uint lineNumber() const { return m_nLineNumber; }

private:
    void growLineBuffer(size_t nGrow);

    typedef QString (MakefileLineReader::*ReadLineImpl)();
    ReadLineImpl m_readLineImpl;
    QString readLine_impl_local8bit();
    QString readLine_impl_unicode();

private:
    QFile m_file;
    QTextStream m_textStream;
    static const size_t m_nInitialLineBufferSize = 6144;
    static const size_t m_nLineBufferGrowSize = 1024;
    size_t m_nLineBufferSize;
    char *m_lineBuffer;
    uint m_nLineNumber;
};

} // namespace NMakeFile

#endif // MAKEFILELINEREADER_H
