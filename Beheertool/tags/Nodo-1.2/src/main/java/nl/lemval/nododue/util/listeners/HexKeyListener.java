/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import javax.swing.JComboBox;
import javax.swing.JTextField;
import nl.lemval.nododue.NodoDueManager;

/**
 *
 * @author Michael
 */
public class HexKeyListener implements KeyListener {

    private int maxSize = 0;

    public HexKeyListener() {
    }

    public HexKeyListener(int size) {
        this.maxSize = size;
    }
    
//    public static void checkHexCode(KeyEvent evt, int maxSize) {
    public void checkHexCode(KeyEvent evt) {
        Object o = evt.getSource();
        JTextField tf = null;
        if ( o instanceof JComboBox ) {
            tf = (JTextField) ((JComboBox) o).getEditor().getEditorComponent();
        }
        if ( o instanceof JTextField ) {
            tf = (JTextField) o;
        }
        if ( tf == null ) {
            // Not supported
            return;
        }

        String newValue = null;
        char value = evt.getKeyChar();
        try {
            newValue = TextFieldUtil.determineValue(tf, value);
        } catch (Exception e) {
            NodoDueManager.showMessage("HexKey.handling_failed", value, "?");
        }

        boolean hexValue = false;
        if ( newValue.startsWith("0x") || newValue.startsWith("0X") ) {
            // Allow hex prefixing
            newValue = newValue.substring(2);
            hexValue = true;
            if ( value == 'X' ) {
                evt.setKeyChar('x');
            }
            if ( newValue.length() == 0 ) {
                return;
            }
        }

        // Now check the validity of the text
        try {
            // Jump to catch block
            if ( hexValue )
                throw new NumberFormatException("Hex value, not decimal");
            
            long result = Long.parseLong(newValue);
            if ( result > getMaxValue() ) {
                evt.consume();
                return;
            }
        } catch (NumberFormatException e) {
            try {
                long result = Long.parseLong(newValue, 16);
                if ( result > getMaxValue() ) {
                    evt.consume();
                    return;
                }
            } catch (NumberFormatException e1) {
                // No decimal and no hex
                evt.consume();
                return;
            }
        }

        // Make it uppercase
        if ( value >= 'a' && value <= 'f' ) {
            evt.setKeyChar((char)((int)value-32));
        }
    }
    public String toDecValue(String data) {
        long value = toValue(data);
        if ( value == -1 )
            return null;
        return Long.toString(value).toUpperCase();
    }

    public String toHexValue(String data) {
        long value = toValue(data);
        if ( value == -1 )
            return null;
        return Long.toHexString(value).toUpperCase();
    }

    public long toValue(String data) {
        if ( data == null ) {
            return -1;
        }

        boolean hexValue = false;
        String value = data.trim();
        if ( value.startsWith("0x") ) {
            hexValue = true;
            value = value.substring(2);
        }
        if ( value.startsWith("x") ) {
            hexValue = true;
            value = value.substring(1);
        }

        long result = 0;
        try {
            if ( hexValue ) {
                // Jump to catch block
                throw new NumberFormatException("Hex value, not decimal");
            }
            result = Long.parseLong(value);
        } catch (NumberFormatException e) {
            try {
                result = Long.parseLong(value, 16);
            } catch (NumberFormatException ex) {
                return -1;
            }
        }
        return result;
    }
    
    public long getMaxValue() {
        return (long) Math.pow(16, maxSize) - 1;
    }

    public void keyTyped(KeyEvent e) {
//        checkHexCode(e, maxSize);
        checkHexCode(e);
    }

    public void keyPressed(KeyEvent e) {
    }

    public void keyReleased(KeyEvent e) {
    }
}
