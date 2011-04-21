/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

import java.util.HashSet;
import javax.swing.ListModel;
import javax.swing.event.ListDataEvent;
import javax.swing.event.ListDataListener;
import nl.lemval.nododue.util.NodoMacro;
import nl.lemval.nododue.util.NodoMacroList;

/**
 *
 * @author Michael
 */
public class MacroListModel implements ListModel {

    private HashSet<ListDataListener> listeners;
    private NodoMacroList list;

    public MacroListModel() {
        listeners = new HashSet<ListDataListener>();
        list = new NodoMacroList();
    }

    public int getSize() {
        return list.size();
    }

    public NodoMacroList getList() {
        return list;
    }

    public Object getElementAt(int index) {
        return list.get(index);
    }

    public void addListDataListener(ListDataListener l) {
        listeners.add(l);
    }

    public void removeListDataListener(ListDataListener l) {
        listeners.remove(l);
    }

    public void setContent(NodoMacroList newlist) {
        this.list = newlist;

        ListDataEvent evt = new ListDataEvent(list, ListDataEvent.CONTENTS_CHANGED, 0, getSize());
        for (ListDataListener listDataListener : listeners) {
            listDataListener.contentsChanged(evt);
        }
    }

    public void remove(Object object) {
        if ( list.remove(object) ) {
            ListDataEvent evt = new ListDataEvent(list, ListDataEvent.CONTENTS_CHANGED, 0, getSize());
            for (ListDataListener listDataListener : listeners) {
                listDataListener.contentsChanged(evt);
            }
        }
    }

    public void add(NodoMacro nodoMacro) {
        if ( list.add(nodoMacro) ) {
            ListDataEvent evt = new ListDataEvent(list, ListDataEvent.CONTENTS_CHANGED, 0, getSize());
            for (ListDataListener listDataListener : listeners) {
                listDataListener.contentsChanged(evt);
            }
        }
    }

    public void move(int idx, boolean moveUp) {
        if ( list.move(idx, moveUp) ) {
            int start = idx;
            int end = idx;
            if ( moveUp ) start--; else end++;
            ListDataEvent evt = new ListDataEvent(list, ListDataEvent.CONTENTS_CHANGED, start, end);
            for (ListDataListener listDataListener : listeners) {
                listDataListener.contentsChanged(evt);
            }
        }
    }
}
