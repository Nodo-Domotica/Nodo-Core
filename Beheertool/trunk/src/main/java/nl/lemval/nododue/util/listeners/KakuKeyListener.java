/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.JComboBox;
import javax.swing.JTextField;
import nl.lemval.nododue.NodoDueManager;

/**
 *
 * @author Michael
 */
public class KakuKeyListener implements KeyListener {

    private static Pattern pattern = Pattern.compile("[A-Pa-p]?([0-9]{0,2})");
    private static Pattern dissect = Pattern.compile("([A-Pa-p])([0-9]{1,2})");
    
    public void checkKakuCode(KeyEvent evt) {
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

        // Determine the text, but ignore any selection made, since that
        // is replaced by the newly typed character.
        String text = tf.getText();
        int idx = tf.getCaretPosition();

        String old = tf.getSelectedText();
        if ( old != null ) {
            text = text.replaceFirst(old, "");
            idx -= old.length();
            if ( idx < 0) { idx = 0; }
        }

        // Now construct the new text
        char value = evt.getKeyChar();
        StringBuilder nv = new StringBuilder(text);

        // Note that deletes and backspaces cannot be consumed, since they
        // are already reflected in the text found..
        if ( value == 8 /*bs*/ ) {
//            nv.deleteCharAt(idx);
        } else if ( value == 127 /*delete*/ ) {
//            nv.delete(idx, idx+1);
        } else {
            try {
                nv.insert(idx, value);
            } catch (Exception e) {
                NodoDueManager.showMessage("HexKey.handling_failed", value, idx);
            }
        }

        Matcher matcher = pattern.matcher(nv.toString());
        if ( matcher.matches() == false ) {
            evt.consume();
            return;
        }

        if ( value >= 'a' && value <= 'p' ) {
            evt.setKeyChar((char)((int)value-32));
        }

        if ( matcher.groupCount() == 0 ) {
            // Accept
            return;
        }

        String addr = matcher.group(1);
        if ( addr == null || addr.length() == 0 ) {
            // Accept
            return;
        }

        try {
            if ( Integer.parseInt(addr) > 16 ) {
                evt.consume();
                return;
            }
        } catch (NumberFormatException e) {
            evt.consume();
            return;
        }
    }

    public boolean checkValue(String data) {
        try {
            Matcher matcher = dissect.matcher(data);
            if ( matcher.matches() ) {
                String code = matcher.group(1);
                String parm = matcher.group(2);
                if ( Integer.parseInt(parm) <= 16  && (code.charAt(0)-'A') < 16 ) {
                    return true;
                }
            }
        } catch (Exception e) {
        }
        return false;
    }

    public void keyTyped(KeyEvent e) {
        checkKakuCode(e);
    }

    public void keyPressed(KeyEvent e) {
    }

    public void keyReleased(KeyEvent e) {
    }
}
