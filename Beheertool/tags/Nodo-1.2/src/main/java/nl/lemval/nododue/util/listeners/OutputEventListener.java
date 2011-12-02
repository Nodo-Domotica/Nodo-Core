/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

import nl.lemval.nododue.cmd.NodoResponse;

/**
 *
 * @author Michael
 */
public interface OutputEventListener {
    public void handleOutputLine(String message);
    public void handleClear();

    public void handleNodoResponses(NodoResponse[] responses);
}
