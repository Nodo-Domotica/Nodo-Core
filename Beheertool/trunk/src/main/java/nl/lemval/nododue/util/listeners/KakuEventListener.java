/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JList;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.component.KakuButton;

/**
 *
 * @author Michael
 */
public class KakuEventListener implements ActionListener {

    private JList channel = null;
    private StatusMessageListener listener = null;

    public KakuEventListener(JList channelList, StatusMessageListener listener) {
        this.channel = channelList;
        this.listener = listener;
    }

    public void actionPerformed(ActionEvent e) {
        if ( NodoDueManager.hasConnection() == false ) {
            return;
        }
        Object o = e.getSource();
        if ( channel != null && o instanceof KakuButton ) {
            String value = (String) channel.getSelectedValue();
            if ( value != null ) {
                NodoCommand command = ((KakuButton)o).getCommand(value);
                NodoDueManager.getApplication()
                    .getSerialCommunicator().send(command);
            } else {
                listener.showStatusMessage(NodoDueManager.getMessage("Kaku.select_home"));
            }
        }
    }

}
