/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.JTextField;

import nl.lemval.nododue.cmd.CommandInfo;
import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.CommandType;
import nl.lemval.nododue.cmd.Parameter;
import nl.lemval.nododue.util.listeners.HexKeyListener;

/**
 *
 * @author Michael
 */
public class SettingsCellHelper {
    private HashMap<String, JComponent> components = new HashMap<String, JComponent>();
    private NodoSettingsTableModel model;
    private HexKeyListener hkl = new HexKeyListener(2);
    private Map<String, CommandInfo> cis = null;

    public SettingsCellHelper(NodoSettingsTableModel model) {
        this.model = model;
    }

    public Component getTableCellComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
	// Only a value field...
	boolean editable = table.getModel().isCellEditable(row, column);
        Component cmp = getComponent(row, column, editable);
	if ( cmp instanceof JComboBox) {
	    ((JComboBox)cmp).setSelectedItem(value);
	} else if ( cmp instanceof JTextField ) {
	    ((JTextField)cmp).setText((String)value);
	} else if ( cmp instanceof JLabel ) {
	    ((JLabel)cmp).setText((String) value);
	}

	Color foreGround = table.getForeground();
	Color backGround = table.getBackground();
	if (isSelected && !editable) {
	    foreGround = table.getSelectionForeground();
	    backGround = table.getSelectionBackground();
	}

	cmp.setForeground(foreGround);
	cmp.setBackground(backGround);

	return cmp;
    }

    private JComponent getComponent(int row, int column, boolean editable) {
        String key = getKey(row, column);
        JComponent component = components.get(key);

	if (component == null) {
	    if ( editable ) {
		CommandInfo ci = getCommands().get(model.getSetting(row).getName());
		Parameter parm = ci.getParameter(column==2?0:1);
		String[] names = parm.getValueNames();
		if ( names.length > 1 ) {
		    JComboBox combo = new JComboBox();
		    DefaultComboBoxModel cbm = (DefaultComboBoxModel) combo.getModel();
		    for (int i = 0; i < names.length; i++) {
			cbm.addElement(names[i]);
		    }
		    component = combo;
		} else {
		    JTextField field = new JTextField();
		    field.setMargin(new java.awt.Insets(1, 3, 1, 3));
		    field.addKeyListener(hkl);
		    component = field;
	            component.setOpaque(true);
		    component.setBackground(Color.red);
		}
		component.setFont(component.getFont().deriveFont(Font.BOLD));
	    } else {
		component = new JLabel();
	        component.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 3, 1, 3));
		component.setOpaque(true);
		component.setBackground(Color.blue);
	    }
            components.put(key, component);
        }
        return component;
    }

    private String getKey(int r, int c) {
        return String.valueOf(31*r + c);
    }

    public Object getValue(int row, int column) {
        JComponent cmp = getComponent(row, column, false);
	if ( cmp instanceof JComboBox) {
	    return ((JComboBox)cmp).getSelectedItem();
	}
	if ( cmp instanceof JTextField ) {
	    return ((JTextField)cmp).getText();
	}
	if ( cmp instanceof JLabel ) {
            return ((JLabel)cmp).getText();
	}
        return "?";
    }

    private Map<String, CommandInfo> getCommands() {
        if ( cis == null ) {
            cis = new HashMap<String, CommandInfo>();
            Collection<CommandInfo> cc = CommandLoader.getActions(CommandType.ALL);
            for (CommandInfo commandInfo : cc) {
                cis.put(commandInfo.getName(), commandInfo);
            }
        }
        return cis;
    }
}
