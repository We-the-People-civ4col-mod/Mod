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

#ifndef DEPENDENCYGRAPH_H
#define DEPENDENCYGRAPH_H

#include <QtCore/QHash>
#include <QtCore/QSet>

namespace NMakeFile {

class DescriptionBlock;

class DependencyGraph
{
public:
    DependencyGraph();
    ~DependencyGraph();

    void build(DescriptionBlock* target);
    bool isEmpty() const;
    void removeLeaf(DescriptionBlock* target);
    DescriptionBlock* findAvailableTarget();
    void dump();
    void dotDump();
    void clear();

private:
    bool isTargetUpToDate(DescriptionBlock* target);

    struct Node
    {
        enum State {UnknownState, ExecutingState};

        State state;
        DescriptionBlock* target;
        QList<Node*> children;
        QList<Node*> parents;
    };

    Node* createNode(DescriptionBlock* target, Node* parent);
    void deleteNode(Node* node);
    void removeLeaf(Node* node);
    void internalBuild(Node* node);
    void addEdge(Node* parent, Node* child);
    void internalDump(Node* node, QString& indent);
    void internalDotDump(Node* node, const QString& parent);
    void displayNodeBuildInfo(Node* node, bool isUpToDate);

private:
    Node* m_root;
    QHash<DescriptionBlock*, Node*> m_nodeContainer;
    QSet<Node*> m_leavesSet;
    QList<Node*> m_leavesList;
    bool m_bDirtyLeaves;
};

} // namespace NMakeFile

#endif // DEPENDENCYGRAPH_H
