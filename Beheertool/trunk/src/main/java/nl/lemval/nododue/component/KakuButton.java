/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

import javax.swing.JButton;

import nl.lemval.nododue.cmd.CommandLoader;
import nl.lemval.nododue.cmd.NodoCommand;
import nl.lemval.nododue.cmd.CommandInfo.Name;

/**
 *
 * @author Michael
 */
public class KakuButton extends JButton {

    private boolean isOnSwitch = false;
    private int code = 1;

    public KakuButton(int i, boolean b) {
        super(b?"On":"Off");
        this.code = i;
        this.isOnSwitch = b;
    }

    public NodoCommand getCommand(String value) {
        NodoCommand cmd = new NodoCommand(CommandLoader.get(Name.SendKAKU), value + code,(isOnSwitch?"On":"Off"));
        cmd.makeDistributed();
        return cmd;
    }
}
