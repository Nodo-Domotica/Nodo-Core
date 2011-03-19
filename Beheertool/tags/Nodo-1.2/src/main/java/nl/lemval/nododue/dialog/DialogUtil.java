/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.dialog;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.KeyStroke;

/**
 *
 * @author Michael
 */
class DialogUtil {

    protected static void configureCloseOnEscape(final JDialog dialog) {
	KeyStroke stroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0);
	dialog.getRootPane().registerKeyboardAction(new ActionListener() {

	    public void actionPerformed(ActionEvent e) {
		dialog.setVisible(false);
	    }
	}, stroke, JComponent.WHEN_IN_FOCUSED_WINDOW);
    }
}
