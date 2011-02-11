/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

import java.awt.Component;
import javax.swing.JTable;
import javax.swing.table.TableCellRenderer;

/**
 *
 * @author Michael
 */
public class SettingsCellRenderer implements TableCellRenderer {

    private SettingsCellHelper helper;

    public SettingsCellRenderer(NodoSettingsTableModel model) {
        helper = new SettingsCellHelper(model);
    }

    public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
        return helper.getTableCellComponent(table, value, isSelected, hasFocus, row, column);
    }
}
