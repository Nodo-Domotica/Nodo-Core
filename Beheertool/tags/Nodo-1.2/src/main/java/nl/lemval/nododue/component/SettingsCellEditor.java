/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

import java.awt.Component;
import javax.swing.AbstractCellEditor;
import javax.swing.JTable;
import javax.swing.table.TableCellEditor;

/**
 *
 * @author Michael
 */
public class SettingsCellEditor extends AbstractCellEditor implements TableCellEditor {

    private SettingsCellHelper helper;
    private int row;
    private int col;

    public SettingsCellEditor(NodoSettingsTableModel model) {
        super();
        helper = new SettingsCellHelper(model);
    }

    public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) {
        this.row = row;
        this.col = column;
        return helper.getTableCellComponent(table, value, isSelected, true, row, column);
    }

    public Object getCellEditorValue() {
//        System.out.println("On " + row + ":" + col + " > " + helper.getValue(row, col));
        return helper.getValue(row,col);
    }
}
