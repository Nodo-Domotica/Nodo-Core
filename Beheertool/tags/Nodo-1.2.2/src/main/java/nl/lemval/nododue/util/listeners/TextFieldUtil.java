/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

import javax.swing.JTextField;

/**
 *
 * @author Michael
 */
public class TextFieldUtil {

    public static String determineValue(JTextField tf, char insert) {
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
        StringBuilder nv = new StringBuilder(text);

        // Note that deletes and backspaces cannot be consumed, since they
        // are already reflected in the text found..
        if ( insert == 8 /*bs*/ ) {
//            nv.deleteCharAt(idx);
        } else if ( insert == 127 /*delete*/ ) {
//            nv.delete(idx, idx+1);
        } else {
            nv.insert(idx, insert);
        }
        return nv.toString();
    }

}
