/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.dialog;

import java.awt.Color;
import java.awt.Component;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JList;
import nl.lemval.nododue.util.Device;

/**
 *
 * @author Michael
 */
class DevicesCellRenderer extends DefaultListCellRenderer {

    @Override
    public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
        Component rv = super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
        if ( value instanceof Device && ((Device) value).isIgnored() ) {
            if ( isSelected ) {
                rv.setBackground(Color.LIGHT_GRAY);
            } else {
                rv.setForeground(Color.GRAY);
            }
        }
        return rv;
    }
}
