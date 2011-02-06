/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;
import nl.lemval.nododue.util.NodoSetting;
import org.jdesktop.application.ResourceMap;

/**
 *
 * @author Michael
 */
public class NodoSettingsTableModel implements TableModel {

    private ArrayList<NodoSetting> settings;
    private HashSet<TableModelListener> listeners;
    private ResourceMap resources;

    public NodoSettingsTableModel(ResourceMap resourceMap) {
        this.resources = resourceMap;
        this.settings = new ArrayList<NodoSetting>();
        this.listeners = new HashSet<TableModelListener>();
    }

    public int getRowCount() {
        return settings.size();
    }

    public int getColumnCount() {
        return 5;
    }

    public NodoSetting getSetting(int row) {
        return settings.get(row);
    }
    
    public String getColumnName(int columnIndex) {
        return resources.getString("tableColumn." + (columnIndex+1));
    }

    public Class<?> getColumnClass(int columnIndex) {
        return String.class;
    }

    public boolean isCellEditable(int rowIndex, int columnIndex) {
        // Only column 2 and 4 are possibly editable
        if ( columnIndex == 2 || columnIndex == 4 ) {
            try {
                NodoSetting setting = settings.get(rowIndex);
                // Non-commands are not editable
                if ( setting.isCommand() ) {
                    if ( setting.hasSecondAttribute() ) {
                        // Having a second attribute means either two
                        // parameters or one with a port indicator.
                        if ( columnIndex==2 && setting.firstAttributeIsFixed() )
                            return false;
                        return true;
                    } else {
                        return (columnIndex == 2);
                    }
                }
            } catch (Exception e) {}
        }
        return false;
    }

    public Object getValueAt(int rowIndex, int columnIndex) {
        NodoSetting setting = settings.get(rowIndex);
        switch (columnIndex) {
            case 0: return setting.getName();
            case 1: return setting.getAttributeName1();
            case 2: return setting.getAttributeValue1();
            case 3: return setting.getAttributeName2();
            case 4: return setting.getAttributeValue2();
        }
        return null;
    }

    public void setValueAt(Object aValue, int rowIndex, int columnIndex) {
        NodoSetting setting = settings.get(rowIndex);
        String data = (String) aValue;
        switch (columnIndex) {
            case 2: setting.setAttributeValue1(data); break;
            case 4: setting.setAttributeValue2(data); break;
        }
    }

    public void addTableModelListener(TableModelListener l) {
        listeners.add(l);
    }

    public void removeTableModelListener(TableModelListener l) {
        listeners.remove(l);
    }

    public void addSetting(NodoSetting setting) {
        if ( settings.contains(setting) ) {
            return;
        }
        settings.add(setting);
        Collections.sort(settings, new Comparator<NodoSetting>() {
            public int compare(NodoSetting o1, NodoSetting o2) {
                int result = o1.getName().compareTo(o2.getName());
                if ( result == 0 ) {
                    try {
                        result = Integer.parseInt(o1.getAttributeValue1()) -
                            Integer.parseInt(o2.getAttributeValue1());
                    } catch (Exception e) {
                    }
                }
                return result;
            }
        });
        for (TableModelListener tableModelListener : listeners) {
            tableModelListener.tableChanged(new TableModelEvent(this));
        }
    }

    public void clear() {
        settings.clear();
    }

    public Collection<NodoSetting> getSettings(boolean all) {
        Collection<NodoSetting> result = new HashSet<NodoSetting>();
        if ( all ) {
            result.addAll(settings);
        } else {
            for (int i = 0; i < settings.size(); i++) {
                if ( isCellEditable(i, 2) || isCellEditable(i, 4)) {
                    result.add(settings.get(i));
                }
            }
        }
        return result;
    }
}
