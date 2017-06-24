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

#include "dependencygraph.h"
#include "makefile.h"
#include "options.h"
#include "fastfileinfo.h"

#include <QFile>
#include <QDebug>
#include <QDir>

namespace NMakeFile {

DependencyGraph::DependencyGraph()
:   m_root(0),
    m_bDirtyLeaves(true)
{
}

DependencyGraph::~DependencyGraph()
{
    clear();
}

DependencyGraph::Node* DependencyGraph::createNode(DescriptionBlock* target, Node* parent)
{
    Node* node = new Node;
    node->target = target;
    node->state = Node::UnknownState;
    if (parent) {
        addEdge(parent, node);
    }

    m_nodeContainer[target] = node;
    return node;
}

void DependencyGraph::deleteNode(Node* node)
{
    m_nodeContainer.remove(node->target);
    if (node == m_root) m_root = 0;
    delete node;
}

void DependencyGraph::build(DescriptionBlock* target)
{
    m_bDirtyLeaves = true;
    m_root = createNode(target, 0);
    internalBuild(m_root);
    //dump();
    //qDebug() << "\n\n-------------------------------------------------\n";

    //do {
    //    target = findAvailableTarget();
    //    if (target) qDebug() << "XXX" << target->m_targetName << target->m_bFileExists;
    //} while(target);

    //qDebug() << "\nFINISHED";
}

bool DependencyGraph::isTargetUpToDate(DescriptionBlock* target)
{
    FastFileInfo fi(target->targetName());
    if (fi.exists()) {
        target->m_bFileExists = true;
        target->m_timeStamp = fi.lastModified();
    }

    bool isUpToDate;
    if (target->m_dependents.isEmpty()) {
        isUpToDate = target->m_bFileExists;
    } else {
        // find latest timestamp of all dependents
        FileTime latestDependentTime;
        foreach (const QString& dependentName, target->m_dependents) {
            FileTime ts;
            DescriptionBlock *dependent = target->makefile()->target(dependentName);
            if (dependent) {
                ts = dependent->m_timeStamp;
                if (!dependent->m_bFileExists && !dependent->m_commands.isEmpty()) {
                    // Mimic insane nmake behaviour: If the dependent is a pseudotarget
                    // and has commands, then this target is out of date.
                    latestDependentTime = FileTime::currentTime();
                    break;
                }
            }

            if (!ts.isValid()) {
                FastFileInfo fi(dependentName);
                if (fi.exists())
                    ts = fi.lastModified();
            }

            if (!ts.isValid()) {
                // File does not exist, so it gets the latest time stamp.
                latestDependentTime = FileTime::currentTime();
                break;
            }

            if (latestDependentTime < ts)
                latestDependentTime = ts;
        }

        if (!target->m_bFileExists)
            target->m_timeStamp = latestDependentTime;

        isUpToDate = (target->m_bFileExists && latestDependentTime <= target->m_timeStamp);
    }

    if (isUpToDate && !target->m_inferenceRules.isEmpty()) {
        // The target is up-to-date but it still has unapplied inference rules.
        // That means there could be dependents we didn't take into account yet.

        QStringList savedDependents = target->m_dependents;
        QVector<InferenceRule*> savedRules = target->m_inferenceRules;
        target->m_inferenceRules.clear();

        bool inferredDependentAdded = false;
        foreach (InferenceRule *rule, savedRules) {
            QString inferredDependent = rule->inferredDependent(target->targetName());
            if (!target->m_dependents.contains(inferredDependent) && FastFileInfo(inferredDependent).exists()) {
                inferredDependentAdded = true;
                target->m_dependents.append(inferredDependent);
            }
        }

        if (inferredDependentAdded)
            isUpToDate = isTargetUpToDate(target);

        target->m_dependents = savedDependents;
        target->m_inferenceRules = savedRules;
    }

    if (!isUpToDate && target->m_bFileExists)
        target->m_timeStamp.clear();

    return isUpToDate;
}

void DependencyGraph::internalBuild(Node* node)
{
    foreach (const QString& dependentName, node->target->m_dependents) {
        Makefile* const makefile = node->target->makefile();
        DescriptionBlock* dependent = makefile->target(dependentName);
        if (!dependent) {
            // We don't know dependent "foo" but it may have been defined as "C:\MySourceDir\foo"
            dependent = makefile->target(makefile->dirPath() + QDir::separator() + dependentName);
        }
        if (!dependent) {
            if (!FastFileInfo(dependentName).exists()) {
                QByteArray msg = "Error: dependent '";
                msg += dependentName.toLocal8Bit();
                msg += "' does not exist.\n";
                fputs(msg.constData(), stderr);
                exit(2);
            }
            continue;
        }

        Node* child;
        if (m_nodeContainer.contains(dependent)) {
            child = m_nodeContainer.value(dependent);
            addEdge(node, child);
        } else
            child = createNode(dependent, node);

        internalBuild(child);
    }

    if (node->children.isEmpty() && !m_leavesSet.contains(node)) {
        m_leavesSet.insert(node);
        m_leavesList.append(node);
    }
}

void DependencyGraph::dump()
{
    QString indent;
    internalDump(m_root, indent);
}

void DependencyGraph::internalDump(Node* node, QString& indent)
{
    puts(qPrintable(QString(indent + node->target->targetName())));
    indent.append(QLatin1Char(' '));
    foreach (Node* child, node->children) {
        internalDump(child, indent);
    }
    indent.resize(indent.length() - 1);
}

void DependencyGraph::dotDump()
{
    printf("digraph G {\n");
    QString parent;
    internalDotDump(m_root, parent);
    printf("}\n");
}

void DependencyGraph::internalDotDump(Node* node, const QString& parent)
{
    if (!parent.isNull()) {
        QByteArray line = "  \"" + parent.toLocal8Bit() + "\" -> \"" + node->target->targetName().toLocal8Bit() + "\";";
        puts(line);
    }
    foreach (Node* child, node->children) {
        internalDotDump(child, node->target->targetName());
    }
}

void DependencyGraph::clear()
{
    m_root = 0;
    qDeleteAll(m_nodeContainer);
    m_nodeContainer.clear();
    m_leavesSet.clear();
    m_leavesList.clear();
}

void DependencyGraph::addEdge(Node* parent, Node* child)
{
    if (!parent->children.contains(child))
        parent->children.append(child);
    if (!child->parents.contains(parent))
        child->parents.append(parent);
}

bool DependencyGraph::isEmpty() const
{
    return m_nodeContainer.isEmpty();
}

void DependencyGraph::removeLeaf(DescriptionBlock* target)
{
    Node* nodeToRemove = m_nodeContainer.value(target);
    if (nodeToRemove)
        removeLeaf(nodeToRemove);
}

void DependencyGraph::removeLeaf(Node* node)
{
    Q_ASSERT(node);
    Q_ASSERT(node->children.isEmpty());

    m_leavesSet.remove(node);
    m_leavesList.removeAll(node);

    QList<Node*>::iterator it;
    foreach (Node* parent, node->parents) {
        it = qFind(parent->children.begin(), parent->children.end(), node);
        parent->children.erase(it);
        if (parent->children.isEmpty()) {
            m_bDirtyLeaves = true;
            m_leavesSet.insert(parent);
            m_leavesList.append(parent);
        }
    }
    deleteNode(node);
}

DescriptionBlock* DependencyGraph::findAvailableTarget()
{
    if (m_leavesSet.isEmpty())
        return 0;

    // remove all leaves that are not up-to-date
    QList<Node*> upToDateNodes;
    while (m_bDirtyLeaves) {
        m_bDirtyLeaves = false;
        foreach (Node *leaf, m_leavesList)
            if (leaf->state != Node::ExecutingState && isTargetUpToDate(leaf->target))
                upToDateNodes.append(leaf);
        foreach (Node *leaf, upToDateNodes) {
            displayNodeBuildInfo(leaf, true);
            removeLeaf(leaf);
        }
        upToDateNodes.clear();
    }

    // apply inference rules separated by makefiles
    QSet<Makefile*> makefileSet;
    QMultiHash<Makefile*, DescriptionBlock*> multiHash;
    foreach (Node *leaf, m_leavesList) {
        makefileSet.insert(leaf->target->makefile());
        multiHash.insert(leaf->target->makefile(), leaf->target);
    }
    foreach (Makefile *mf, makefileSet)
        mf->applyInferenceRules(multiHash.values(mf));

    // return the first leaf that is not currently executed
    foreach (Node *leaf, m_leavesList) {
        if (leaf->state != Node::ExecutingState) {
            leaf->state = Node::ExecutingState;
            displayNodeBuildInfo(leaf, false);
            return leaf->target;
        }
    }

    return 0;
}

void DependencyGraph::displayNodeBuildInfo(Node* node, bool isUpToDate)
{
    if (node->target->makefile()->options()->displayBuildInfo) {
        QByteArray msg;
        if (isUpToDate)
            msg = " ";
        else
            msg = "*";
        msg += node->target->m_timeStamp.toString().toLocal8Bit() + " " +
               node->target->targetName().toLocal8Bit();
        puts(msg);
    }
}

} // namespace NMakeFile
